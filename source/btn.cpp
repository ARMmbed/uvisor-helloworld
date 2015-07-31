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
#include "btn.h"
#include "main-acl.h"
#include "box-challenge.h"

InterruptIn btn(MAIN_BTN);

static void btn_onpress(void)
{
    uint8_t challenge[CHALLENGE_SIZE];

    /* attempt copy from g_BLA (owned by box_challenge) and
     *                   g_BLA (owned by the mbed box) */
    /* TODO */
}

void btn_init(void)
{
    /* configure pushbutton */
    btn.mode(PullUp);
    wait(.01);
    btn.fall(&btn_onpress);
}
