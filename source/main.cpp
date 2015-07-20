/***************************************************************
 * This confidential and  proprietary  software may be used only
 * as authorised  by  a licensing  agreement  from  ARM  Limited
 *
 *             (C) COPYRIGHT 2013-2015 ARM Limited
 *                      ALL RIGHTS RESERVED
 *
 *  The entire notice above must be reproduced on all authorised
 *  copies and copies  may only be made to the  extent permitted
 *  by a licensing agreement from ARM Limited.
 *
 ***************************************************************/
#include <mbed/mbed.h>
#include <uvisor-lib/uvisor-lib.h>
#include "box-challenge.h"

/* enable uvisor */
UVISOR_SET_MODE(2);

DigitalOut led(LED1);

int main(void)
{
    int i;
    uint8_t challenge[CHALLENGE_SIZE];

    /* reset challenge */
    memset(&challenge, 0, sizeof(challenge));

    /* turn LED off */
    led = false;

    while(1) {

        /* check for secret, blink if it's wrong */
        if(verify_secret(challenge, sizeof(challenge)))
            led = true;
        else
            led = !led;

        /* spinning wait, FIXME: replace by timer-wait */
        for(i = 0; i < 10000000; ++i);
    }
}
