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
#include <uvisor-lib/uvisor-lib.h>
#include "box_secure_print.h"
#include "box_benchmark.h"

/* create ACLs for secret data section */
static const UvisorBoxAclItem g_main_acl[] = {
    {MCG,                  sizeof(*MCG),       UVISOR_TACLDEF_PERIPH},
    {SIM,                  sizeof(*SIM),       UVISOR_TACLDEF_PERIPH},
    {(void *) 0x40048000,  sizeof(*SIM),       UVISOR_TACLDEF_PERIPH},
    {PORTA,                sizeof(*PORTA),     UVISOR_TACLDEF_PERIPH},
    {PORTB,                sizeof(*PORTB),     UVISOR_TACLDEF_PERIPH},
    {PORTC,                sizeof(*PORTC),     UVISOR_TACLDEF_PERIPH},
    {PIT,                  sizeof(*PIT),       UVISOR_TACLDEF_PERIPH},
};

/* enable uvisor */
UVISOR_SET_MODE_ACL(2, g_main_acl);

int main(void) {
    /************************************************************
     * BENCHMARK
     ************************************************************/
    /* initialize benchmark unit */
    uvisor_benchmark_configure();

    /* call mock function with and without secure gateway */
    uint32_t t1 = benchmark_mock(secure);
    uint32_t t2 = benchmark_mock(unsecure);

    /* assemble message string */
    char buffer[60];
    sprintf(buffer, "Function call overhead: %lu clock cycles\n\r", t1 - t2);
    secure_print_msg(buffer, 60);
    /************************************************************/

    /* initialize the secure timers */
    secure_timer_init();

    while(1)
    {
        while(!g_polling)
        {
            __WFE();
        }
        secure_print_pwd();
        g_polling = 0;
    }
    return 0;
}
