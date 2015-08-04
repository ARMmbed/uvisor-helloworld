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
#include "mbed/mbed.h"
#include "uvisor-lib/uvisor-lib.h"
#include "main-hw.h"
#include "box-challenge.h"
#include "btn.h"

/* create ACLs for main box */
MAIN_ACL(g_main_acl);

/* enable uvisor */
UVISOR_SET_MODE_ACL(UVISOR_ENABLED, g_main_acl);

DigitalOut led(MAIN_LED);

void app_start(int, char *[])
{
    uint8_t challenge[CHALLENGE_SIZE];

    /* reset challenge */
    memset(&challenge, 0, sizeof(challenge));

    /* turn LED off */
    led = false;

    /* configure pushbutton */
    btn_init();

    while(1) {

        /* check for secret, blink if it's wrong */
        if(verify_secret(challenge, sizeof(challenge)))
            led = true;

        /* wait before trying again */
        wait(1.0);
    }
}
