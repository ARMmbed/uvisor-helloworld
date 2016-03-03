/*
 * Copyright (c) 2016, ARM Limited, All Rights Reserved
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
#include "uvisor-lib/uvisor-lib.h"
#include "box-debug.h"
#include "main-hw.h"

/* Delay between single blinks of a pattern. Patterns are repeated multiple
 * times, with a delay of twice the number below between them. */
#define BOX_DEBUG_LED_DELAY 0x400000

/* Configure the debug box. */
UVISOR_BOX_NAMESPACE(NULL);
UVISOR_BOX_CONFIG(box_debug, UVISOR_BOX_STACK_SIZE);

/* This handler returns the driver version used for the debug box.
 * uVisor will check this version against the available ones. */
static uint32_t get_version(void)
{
    return 0;
}

/* This handler emits a blinking LED pattern for three times. A blinking LED
 * pattern is made of as many blinks as the value of the error reason, provided
 * by uVisor. */
static void halt_error(int reason)
{
    int i, k;
    int volatile j;
    static DigitalOut halt_led(LED_RED);

    halt_led = LED_OFF;
    for (k = 0; k < 3; k++) {
        for (i = 0; i < reason; i++) {
            for (j = 0; j < BOX_DEBUG_LED_DELAY; j++);
            halt_led = LED_ON;
            for (j = 0; j < BOX_DEBUG_LED_DELAY; j++);
            halt_led = LED_OFF;
        }
        for (j = 0; j < (2 * BOX_DEBUG_LED_DELAY); j++);
    }

    /* uVisor will reboot upon return. */
    return;
}

UVISOR_EXTERN void __box_debug_init(void)
{
    /* Debug box driver -- Version 0 */
    static const TUvisorDebugDriver driver = {
        get_version,
        halt_error
    };

    /* Register the debug box with uVisor. */
    uvisor_debug_init(&driver);
}

void box_debug::init(void)
{
    /* The debug box is initialized from the context of box_debug. */
    secure_gateway(box_debug, __box_debug_init);
}
