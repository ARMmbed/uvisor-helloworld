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
#include "mbed/mbed.h"
#include "uvisor-lib/uvisor-lib.h"

/* enable uvisor */
UVISOR_SET_MODE(2);

DigitalOut led(LED1);

int main(void)
{
    int i;

    led = 0;

    while(1) {
        led = !led;
        for(i = 0; i < 10000000; ++i);
    }
}
