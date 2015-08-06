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

uint8_t g_challenge[CHALLENGE_SIZE];

void app_start(int, char *[])
{
    /* reset challenge */
    memset(&g_challenge, 0, sizeof(g_challenge));

    /* turn LED off */
    led = LED_OFF;

    /* configure pushbutton */
    btn_init();

    while(1) {
        /* check for secret */
        if(verify_secret(g_challenge, sizeof(g_challenge)))
        {
            /* send LED pulse if the secret was found */
            led = LED_ON;
            wait(.4);
            led = LED_OFF;
        }

        /* wait before trying again */
        wait(2.0);
    }
}
