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
#include <uvisor-lib/uvisor-lib.h>
#include "box_secure_print.h"

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

void secure_print_pwd(void)
{
    /* security transition happens here
     *   ensures that __secure_print_pwd() will run with the privileges
     *   of the secure_print box - if uvisor-mode */
    if(__uvisor_mode)
        secure_gateway(secure_print_box, __secure_print_pwd);
    else
        /* fallback for disabled uvisor */
        __secure_print_pwd();
}
