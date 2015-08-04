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
#ifndef __BOX_CHALLENGE_HW_H__
#define __BOX_CHALLENGE_HW_H__

/* target specifc ACLs */
#if   defined(TARGET_LIKE_FRDM_K64F_GCC)

#define BOX_CHALLENGE_ACL(acl_list_name) \
    static const UvisorBoxAclItem g_box_acl[] = {       \
        {RNG,   sizeof(*RNG),   UVISOR_TACLDEF_PERIPH}  \
    }

#elif defined(TARGET_LIKE_STM32F429I_DISCO_GCC)

#define BOX_CHALLENGE_ACL(acl_list_name) \
    static const UvisorBoxAclItem g_box_acl[] = {       \
        {RNG,   sizeof(*RNG),   UVISOR_TACLDEF_PERIPH}  \
    }

#else

#define BOX_CHALLENGE_ACL(acl_list_name) \
    static const UvisorBoxAclItem acl_list_name[] = {}

#endif

#endif/*__BOX_CHALLENGE_HW_H__*/
