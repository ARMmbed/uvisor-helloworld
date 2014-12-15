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

/* enable uvisor */
UVISOR_SET_MODE(2);

int main(void) {
    volatile int t;
    while(1) {
        for (t = 0; t < 2000000; t++);
        secure_print();
    }

    return 0;
}
