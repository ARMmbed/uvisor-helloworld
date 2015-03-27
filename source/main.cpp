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


/* create ACLs for secret data section */
static const UvisorBoxAclItem g_main_acl[] = {
	{MCG,                  sizeof(*MCG),       UVISOR_TACLDEF_PERIPH},
	{SIM,                  sizeof(*SIM),       UVISOR_TACLDEF_PERIPH},
	{PORTA,                sizeof(*PORTA),     UVISOR_TACLDEF_PERIPH},
	{PORTB,                sizeof(*PORTB),     UVISOR_TACLDEF_PERIPH},
	{PORTC,                sizeof(*PORTC),     UVISOR_TACLDEF_PERIPH},
	{PIT,                  sizeof(*PIT),       UVISOR_TACLDEF_PERIPH},
};

/* enable uvisor */
UVISOR_SET_MODE_ACL(2, g_main_acl);

int main(void) {
    volatile int t;
    while(1) {
        for (t = 0; t < 2000000; t++);
        secure_print();
    }

    return 0;
}
