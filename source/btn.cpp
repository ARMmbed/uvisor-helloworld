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
#include "btn.h"
#include "main-hw.h"
#include "box-challenge.h"

InterruptIn btn(MAIN_BTN);

static void btn_onpress(void)
{
    /* attempt copy from box_challenge context
     * we know the context is properly aligned so we try to obtain a carbon copy
     * of its memory location */
    memcpy(&g_challenge, g_box_context, sizeof(g_challenge));
}

void btn_init(void)
{
    /* configure pushbutton */
    btn.mode(PullUp);
    wait(.01);
    btn.fall(&btn_onpress);
}
