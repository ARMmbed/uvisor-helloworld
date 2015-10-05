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
minar::Scheduler *g_scheduler;
minar::callback_handle_t g_event = NULL;

static void toggle_led(void)
{
    led = !led;
}

static void retry_secret(void)
{
    /* check secret */
    bool verified = verify_secret(g_challenge, sizeof(g_challenge));

    /* cancel previous event for LED blinking */
    g_scheduler->cancelCallback(g_event);

    /* schedule new LED blinking pattern */
    /* the blinking frequency will be faster if the secret has been verified */
    g_event = minar::Scheduler::postCallback(FunctionPointer0<void>(toggle_led).bind())
                .period(minar::milliseconds(verified ? 100 : 500))
                .tolerance(minar::milliseconds(1))
                .getHandle();
}

void app_start(int, char *[])
{
    /* reset challenge */
    memset(&g_challenge, 0, sizeof(g_challenge));

    /* turn LED off */
    led = LED_OFF;

    /* configure pushbutton */
    btn_init();

    /* get handle of scheduler */
    g_scheduler = minar::Scheduler::instance();

    /* schedule event to periodically check for secret */
    minar::Scheduler::postCallback(FunctionPointer0<void>(retry_secret).bind())
        .period(minar::milliseconds(1000))
        .tolerance(minar::milliseconds(100));
}
