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
#ifndef __MAIN_ACL_H__
#define __MAIN_ACL_H__

/* target specifc ACLs */
#if   defined(TARGET_LIKE_FRDM_K64F_GCC)

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

#define MAIN_LED LED1
#define MAIN_BTN USER_BUTTON
#define MAIN_ACL(acl_list_name)                         \
    static const UvisorBoxAclItem g_main_acl[] = {      \
    }

#else

#define MAIN_LED NC
#define MAIN_BTN NC
#define MAIN_ACL(acl_list_name) \
    static const UvisorBoxAclItem acl_list_name[] = {}

#endif

#endif/*__MAIN_ACL_H__*/
