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
#include <mbed-hal-ksdk-mcu/TARGET_KSDK_CODE/hal/pit/fsl_pit_hal.h>
#include <mbed-hal-ksdk-mcu/TARGET_KSDK_CODE/drivers/clock/fsl_clock_manager.h>
#include <uvisor-lib/uvisor-lib.h>
#include "box_secure_print.h"

#define TIMER1_PERIOD_US 1000000U
#define TIMER2_PERIOD_US 2000000U

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

/* polling variable (unprotected) */
volatile int g_polling;

/* simple print function for the serial interface */
void serial_printf(char *c)
{
    int data;

    while ((data = (*c++)) != 0) {
        g_data.serial->putc(data);
    }
}

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
    PIT_HAL_SetIntCmd(PIT_BASE, 2, true);

    /* wait for timer 2 to synchronize */
    /* Note: g_data.sync_timer is protected by uVisor */
    serial_printf("  Synchronizing timers...\n\r");
    g_data.sync_timer = 0;
    while(g_data.sync_timer != SYNC_TIMER_VALUE)
    {
        __WFE();
    }
    g_data.sync_timer = 0;
    serial_printf("  ...timers synchronized\n\r");

    /* polling variable */
    /* Note: g_polling is not protected by uVisor */
    g_polling = 1;

    /* disable timer 2 interrupts (timer restarts automatically) */
    PIT_HAL_SetIntCmd(PIT_BASE, 2, false);

    /* clear interrupt flag for timer 1 */
    PIT_HAL_ClearIntFlag(PIT_BASE, 1);
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
    PIT_HAL_ClearIntFlag(PIT_BASE, 2);
}

/* initialize secure timer
 *   if called directly, no security breach as it will run with the
 *   privileges of the caller*/
extern "C" void __secure_timer_init(void)
{
    uint64_t pit_src_clk;
    uint32_t cnt;

    /* initialize serial object on first use */
    if(!g_data.serial) {
        g_data.serial = ::new((void *) &g_data.serial_data)
                        RawSerial(USBTX, USBRX);
        g_data.serial->baud(115200);
    }

    /* enable clock for PIT module */
    CLOCK_SYS_EnablePitClock(0);

    /* turn on the PIT module (freeze during debug) */
    PIT_HAL_Enable(PIT_BASE);
    PIT_HAL_SetTimerRunInDebugCmd(PIT_BASE, false);

    /* PIT module clock */
    pit_src_clk  = CLOCK_SYS_GetPitFreq(0);

    /***************
     * timer 1
     ***************/
    /* configure protected interrupts (priority level 2) */
    uvisor_set_isr(PIT1_IRQn, (uint32_t) &secure_timer1_handler, 0);
    uvisor_set_priority(PIT1_IRQn, 2);
    uvisor_enable_irq(PIT1_IRQn);

    /* configure registers */
    cnt = (uint32_t) (TIMER1_PERIOD_US * pit_src_clk / 1000000U - 1U);
    PIT_HAL_SetTimerPeriodByCount(PIT_BASE, 1, cnt);
    PIT_HAL_StartTimer(PIT_BASE, 1);

    /***************
     * timer 2
     ***************/
    /* configure protected interrupts (priority level 1) */
    uvisor_set_isr(PIT2_IRQn, (uint32_t) &secure_timer2_handler, 0);
    uvisor_set_priority(PIT2_IRQn, 1);
    uvisor_enable_irq(PIT2_IRQn);

    /* configure registers */
    cnt = (uint32_t) (TIMER2_PERIOD_US * pit_src_clk / 1000000U - 1U);
    PIT_HAL_SetTimerPeriodByCount(PIT_BASE, 2, cnt);
    PIT_HAL_StartTimer(PIT_BASE, 2);

    /* interrupts enabled only for timer 1 */
    PIT_HAL_SetIntCmd(PIT_BASE, 1, true);

    serial_printf("  Timers initialized\n\r");
}

/* FIXME implement security context transition */
void secure_timer_init(void)
{
    if(__uvisor_mode)
        secure_gateway(secure_print_box, __secure_timer_init);
    else
        /* fallback for disabled uvisor */
        __secure_timer_init();
}

/* print a secret message
 *   if called directly, no security breach as it will run with the
 *   privileges of the caller*/
extern "C" void __secure_print_pwd(void)
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
void secure_print_pwd(void)
{
    /* security transition happens here
     *   ensures that __secure_print() will run with the privileges
     *   of the secure_print box - if uvisor-mode */
    if(__uvisor_mode)
        secure_gateway(secure_print_box, __secure_print_pwd);
    else
        /* fallback for disabled uvisor */
        __secure_print_pwd();
}

/* print a custom message
 *   if called directly, no security breach as it will run with the
 *   privileges of the caller*/
extern "C" void __secure_print_msg(char *buffer, int len)
{
    int i;

    /* initialize serial object on first use */
    if(!g_data.serial) {
        g_data.serial = ::new((void *) &g_data.serial_data)
                        RawSerial(USBTX, USBRX);
        g_data.serial->baud(115200);
    }

    /* print message if secure*/
    for(i = 0; i < len; i++)
    {
        if(buffer[i] == '\0')
        {
            serial_printf(buffer);
            return;
        }
    }
}

/* FIXME implement security context transition */
void secure_print_msg(char *buffer, int len)
{
    /* security transition happens here
     *   ensures that __secure_print() will run with the privileges
     *   of the secure_print box - if uvisor-mode */
    if(__uvisor_mode)
        secure_gateway(secure_print_box, __secure_print_msg, buffer, len);
    else
        /* fallback for disabled uvisor */
        __secure_print_msg(buffer, len);
}
