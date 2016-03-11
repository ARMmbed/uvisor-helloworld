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
#ifndef __UVISOR_HELLOWORLD_BOX_CHALLENGE_HW_H__
#define __UVISOR_HELLOWORLD_BOX_CHALLENGE_HW_H__

/* Target Specifc ACLs */
#if   defined(TARGET_LIKE_FRDM_K64F)

#define BOX_CHALLENGE_ACL(acl_list_name) \
    static const UvisorBoxAclItem acl_list_name[] = {   \
        {RNG,   sizeof(*RNG),   UVISOR_TACLDEF_PERIPH}  \
    }

#elif defined(TARGET_LIKE_STM32F429I_DISCO)

#define BOX_CHALLENGE_ACL(acl_list_name) \
    static const UvisorBoxAclItem acl_list_name[] = {   \
        {RNG,   sizeof(*RNG),   UVISOR_TACLDEF_PERIPH}  \
    }

#else

#define BOX_CHALLENGE_ACL(acl_list_name) \
    static const UvisorBoxAclItem acl_list_name[] = {}

#endif

#endif/*__UVISOR_HELLOWORLD_BOX_CHALLENGE_HW_H__*/
