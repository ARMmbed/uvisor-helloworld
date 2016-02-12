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
#include "mbed-drivers/mbed.h"
#include "minar/minar.h"
#include "uvisor-lib/uvisor-lib.h"
#include "btn.h"
#include "main-hw.h"
#include "box-challenge.h"

using mbed::util::FunctionPointer0;

InterruptIn btn(MAIN_BTN);
extern Serial pc;

static void btn_onpress(void)
{
    int box_id;
    int calling_box;
#define SOME_SIZE 100
    char box_namespace[SOME_SIZE];
    char calling_box_namespace[SOME_SIZE];

    /* Initialize the namespace strings to blank. */
    box_namespace[0] = '\0';
    calling_box_namespace[0] = '\0';

    box_id = uvisor_box_id_self();
    calling_box = uvisor_box_id_caller();
    uvisor_box_namespace(box_id, box_namespace, SOME_SIZE);
    if (calling_box >= 0) {
        uvisor_box_namespace(calling_box, calling_box_namespace, SOME_SIZE);
    } else {
        strncpy(calling_box_namespace, "Unknown", SOME_SIZE);
    }

    /* attempt copy from box_challenge context
     * we know the context is properly aligned so we try to obtain a carbon copy
     * of its memory location */
    pc.printf("attempting to read the secret from box %d '%s' (called from box %d '%s')...", box_id, box_namespace, calling_box, calling_box_namespace);
    memcpy(&g_challenge, g_box_context, sizeof(g_challenge));
    pc.printf(" done\n\r");
}

static void btn_set_fall(void)
{
    btn.fall(&btn_onpress);
}

void btn_init(void)
{
    /* configure pushbutton */
    btn.mode(MAIN_BTN_PUPD);

    /* Register the button fall handler after a short delay. */
    minar::Scheduler::postCallback(FunctionPointer0<void>(btn_set_fall).bind())
        .delay(minar::milliseconds(10))
        .tolerance(minar::milliseconds(1));

    /* Set a callback to do the equivalent of pushing the button for us. Every
     * period, press the button automatically. Pressing the button
     * automatically after a certain period of time allows us to trigger button
     * presses even for platforms without physically pressable buttons. */
    minar::Scheduler::postCallback(FunctionPointer0<void>(btn_onpress).bind())
        .period(minar::milliseconds(10000))
        .tolerance(minar::milliseconds(100));
}
