/*
 * Copyright (c) 2013-2015, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed/mbed.h"
#include "minar/minar.h"
#include "core-util/FunctionPointer.h"
#include "uvisor-lib/uvisor-lib.h"
#include "main-hw.h"
#include "box-challenge.h"
#include "btn.h"

using mbed::util::FunctionPointer0;

/* create ACLs for main box */
MAIN_ACL(g_main_acl);

/* enable uvisor */
UVISOR_SET_MODE_ACL(UVISOR_ENABLED, g_main_acl);

DigitalOut led(MAIN_LED);

uint8_t g_challenge[CHALLENGE_SIZE];

static void flick_led_off(void)
{
    led = LED_OFF;
}

static void flick_led_on(void)
{
    uint32_t delay;

    /* turn LED on */
    led = LED_ON;

    /* check for secret */
    delay = verify_secret(g_challenge, sizeof(g_challenge)) ? 500 : 10;

    /* schedule disabling of LED */
    minar::Scheduler::postCallback(FunctionPointer0<void>(flick_led_off).bind())
        .delay(minar::milliseconds(delay))
        .tolerance(minar::milliseconds(1));
}

void app_start(int, char *[])
{
    /* reset challenge */
    memset(&g_challenge, 0, sizeof(g_challenge));

    /* turn LED off */
    led = LED_OFF;

    /* configure pushbutton */
    btn_init();

    /* toggle LED */
    minar::Scheduler::postCallback(FunctionPointer0<void>(flick_led_on).bind())
        .period(minar::milliseconds(1000))
        .tolerance(minar::milliseconds(100));
}
