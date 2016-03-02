/*
 * Copyright (c) 2013-2016, ARM Limited, All Rights Reserved
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
#include "mbed-drivers/mbed.h"
#include "minar/minar.h"
#include "core-util/FunctionPointer.h"
#include "uvisor-lib/uvisor-lib.h"
#include "main-hw.h"
#include "box-challenge.h"
#include "box-debug.h"
#include "btn.h"

using mbed::util::FunctionPointer0;

/* Create ACLs for main box. */
MAIN_ACL(g_main_acl);

/* Enable uVisor. */
UVISOR_SET_MODE_ACL(UVISOR_ENABLED, g_main_acl);

DigitalOut led(MAIN_LED);
Serial pc(STDIO_UART_TX, STDIO_UART_RX);

uint8_t g_challenge[CHALLENGE_SIZE];
minar::Scheduler *g_scheduler;
minar::callback_handle_t g_event = NULL;

static void toggle_led(void)
{
    led = !led;
}

static void retry_secret(void)
{
    /* Check the secret. */
    pc.printf("verifying secret...");
    bool verified = challenge_verify(g_challenge, sizeof(g_challenge));
    pc.printf(" done\n\r");

    /* Cancel the previous event for LED blinking. */
    g_scheduler->cancelCallback(g_event);

    /* Schedule the new LED blinking pattern. */
    /* The blinking frequency will be faster if the secret has been verified. */
    g_event = minar::Scheduler::postCallback(FunctionPointer0<void>(toggle_led).bind())
                .period(minar::milliseconds(verified ? 100 : 500))
                .tolerance(minar::milliseconds(1))
                .getHandle();
}

void app_start(int, char *[])
{
    /* Set the console baud-rate. */
    pc.baud(115200);

    pc.printf("***** uvisor-helloworld example *****\n\r");

    /* Initialize the debug box. */
    box_debug::init();

    /* Reset the challenge. */
    memset(&g_challenge, 0, sizeof(g_challenge));

    /* Turn the LED off. */
    led = LED_OFF;

    /* Configure the pushbutton. */
    btn_init();

    /* Get a handle to the scheduler. */
    g_scheduler = minar::Scheduler::instance();

    /* Schedule an event to periodically check for the secret. */
    minar::Scheduler::postCallback(FunctionPointer0<void>(retry_secret).bind())
        .period(minar::milliseconds(1000))
        .tolerance(minar::milliseconds(100));

    pc.printf("main unprivileged box configured\n\r");
}
