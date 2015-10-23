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
