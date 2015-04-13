/***************************************************************
 * This confidential and  proprietary  software may be used only
 * as authorised  by  a licensing  agreement  from  ARM  Limited
 *
 *             (C) COPYRIGHT 2013-2014 ARM Limited
 *                      ALL RIGHTS RESERVED
 *
 *  The entire notice above must be reproduced on all authorised
 *  copies and copies  may only be made to the  extent permitted
 *  by a licensing agreement from ARM Limited.
 *
 ***************************************************************/
#include <mbed.h>
#include <memory>
#include <mbed-hal-k64f/MK64F12/fsl_sim_hal_K64F12.h>
#include <uvisor-lib/uvisor-lib.h>
#include "box_secure_print.h"

#define SYNC_TIMER_VALUE 42

/* setup secret const section (Flash) */
UVISOR_SECURE_CONST char g_password[] = "password";

/* setup secret data section (SRAM) */
typedef struct {
	RawSerial *serial;
	uint8_t serial_data[sizeof(RawSerial)];
    volatile uint32_t sync_timer;
} MyConfig;
UVISOR_SECURE_BSS MyConfig g_data;

/* create ACLs for secret data section */
static const UvisorBoxAclItem g_box_acl[] = {
	{(void *) &g_password, sizeof(g_password), UVISOR_TACLDEF_SECURE_CONST},
	{&g_data,              sizeof(g_data),     UVISOR_TACLDEF_SECURE_BSS},
	{UART0,                sizeof(*UART0),     UVISOR_TACLDEF_PERIPH},
};
/* configure secure box compartnent */
UVISOR_BOX_CONFIG(secure_print_box, g_box_acl, UVISOR_BOX_STACK_SIZE);

/* simple print function for the serial interface */
void serial_printf(const char *c)
{
	int data;

	while ((data = (*c++)) != 0) {
		g_data.serial->putc(data);
	}
}

/* initialize secure timer
 *   if called directly, no security breach as it will run with the
 *   privileges of the caller*/
void secure_timer1_handler(void)
{
    serial_printf("  Timer 1 ISR\n\r");

    /* enable timer 2 interrupts */
    PIT_TCTRL2 |= (1 << 1);

    /* wait for timer 2 to synchronize */
    /* Note: g_data.sync_timer is protected by uVisor */
    serial_printf("  Synchronizing timers...\n\r");
    g_data.sync_timer = 0;
    while(g_data.sync_timer != SYNC_TIMER_VALUE);
    g_data.sync_timer = 0;
    serial_printf("  ...timers synchronized\n\r");

    /* disable timer 2 interrupts (timer restarts automatically) */
    PIT_TCTRL2 &= ~(1 << 1);

    /* clear interrupt flag for timer 1 */
    PIT_TFLG1 |= 1;
}

/* initialize secure timer
 *   if called directly, no security breach as it will run with the
 *   privileges of the caller*/
void secure_timer2_handler(void)
{
    serial_printf("    Timer 2 ISR\n\r");

    /* update sync_timer */
    /* Note: g_data.sync_timer is protected by uVisor */
    g_data.sync_timer = SYNC_TIMER_VALUE;

    /* clear interrupt flag (timer restarts automatically) */
    PIT_TFLG2 |= 1;
}

/* initialize secure timer
 *   if called directly, no security breach as it will run with the
 *   privileges of the caller*/
extern "C" void __secure_timer_init(void)
{
	/* initialize serial object on first use */
	if(!g_data.serial) {
		g_data.serial = ::new((void *) &g_data.serial_data)
		                RawSerial(USBTX, USBRX);
		g_data.serial->baud(115200);
	}

    /* enable clock for PIT module */
    SIM_HAL_EnablePitClock(SIM_BASE, 0);

    /* turn on the PIT module (no freeze during debug) */
    PIT_MCR = 0;

    /* bus clock used here, 60MHz */

    /***************
     * timer 1
     ***************/
    /* configure protected interrupts (priority level 2) */
    uvisor_set_isr(PIT1_IRQn, (uint32_t) &secure_timer1_handler, 0);
    uvisor_set_priority(PIT1_IRQn, 2);
    uvisor_enable_irq(PIT1_IRQn);

    /* configure registers */
    PIT_LDVAL1  = 0x039386FF;   /* 1s */
    PIT_TCTRL1 |= 1;            /* start timer 1 */

    /***************
     * timer 2
     ***************/
    /* configure protected interrupts (priority level 1) */
    uvisor_set_isr(PIT2_IRQn, (uint32_t) &secure_timer2_handler, 0);
    uvisor_set_priority(PIT2_IRQn, 1);
    uvisor_enable_irq(PIT2_IRQn);

    /* configure registers */
    PIT_LDVAL2  = 0x0ABA94FF;   /* 2s */
    PIT_TCTRL2 |= 1;            /* start timer 2 */

    /* interrupts enabled only for timer 1 */
    PIT_TCTRL1 |= (1 << 1);     /* enable timer 1 interrupt */

    serial_printf("  Timers initialized\n\r");
}

/* FIXME implement security context transition */
void secure_timer_init(void)
{
	if(__uvisor_mode)
		secure_gateway(secure_print_box, __secure_timer_init, 0, 0, 0, 0);
	else
		/* fallback for disabled uvisor */
		__secure_timer_init();
}

/* print a secret message
 *   if called directly, no security breach as it will run with the
 *   privileges of the caller*/
extern "C" void __secure_print(void)
{
	/* initialize serial object on first use */
	if(!g_data.serial) {
		g_data.serial = ::new((void *) &g_data.serial_data)
		                RawSerial(USBTX, USBRX);
		g_data.serial->baud(115200);
	}

	/* print secure string */
	serial_printf("The password is: ");
	serial_printf((const char *) g_password);
	serial_printf("\n\r");
}

/* FIXME implement security context transition */
void secure_print(void)
{
	/* security transition happens here
	 *   ensures that __secure_print() will run with the privileges
	 *   of the secure_print box - if uvisor-mode */
	if(__uvisor_mode)
		secure_gateway(secure_print_box, __secure_print, 0, 0, 0, 0);
	else
		/* fallback for disabled uvisor */
		__secure_print();
}
