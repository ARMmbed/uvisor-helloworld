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
#ifndef __MAIN_HW_H__
#define __MAIN_HW_H__

/* the vector containing the challenge is shared with the push-button ISR, so
 * that it can attempt to access it from an IRQ context */
#include "box-challenge.h"
extern uint8_t g_challenge[CHALLENGE_SIZE];

/* target specifc ACLs */
#if   defined(TARGET_LIKE_FRDM_K64F_GCC)

#define LED_ON  false
#define LED_OFF true
#define MAIN_LED LED_BLUE
#define MAIN_BTN SW2
#define MAIN_ACL(acl_list_name) \
    static const UvisorBoxAclItem g_main_acl[] = {      \
        {MCG,    sizeof(*MCG),    UVISOR_TACLDEF_PERIPH}, \
        {SIM,    sizeof(*SIM),    UVISOR_TACLDEF_PERIPH}, \
        {PORTB,  sizeof(*PORTB),  UVISOR_TACLDEF_PERIPH}, \
        {PORTC,  sizeof(*PORTC),  UVISOR_TACLDEF_PERIPH}, \
        {RTC,    sizeof(*RTC),    UVISOR_TACLDEF_PERIPH}, \
        {LPTMR0, sizeof(*LPTMR0), UVISOR_TACLDEF_PERIPH}, \
        {PIT,    sizeof(*PIT),    UVISOR_TACLDEF_PERIPH}, \
    }

#elif defined(TARGET_LIKE_STM32F429I_DISCO_GCC)

#define LED_ON  false
#define LED_OFF true
#define MAIN_LED LED1
#define MAIN_BTN USER_BUTTON
#define MAIN_ACL(acl_list_name)                           \
    static const UvisorBoxAclItem g_main_acl[] = {        \
        {RCC,    sizeof(*RCC),    UVISOR_TACLDEF_PERIPH}, \
    }

#else

#define MAIN_LED NC
#define MAIN_BTN NC
#define MAIN_ACL(acl_list_name) \
    static const UvisorBoxAclItem acl_list_name[] = {}

#endif

#endif/*__MAIN_HW_H__*/
