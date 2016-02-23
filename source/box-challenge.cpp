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
#include "box-challenge.h"
#include "box-challenge-hw.h"

typedef struct {
    uint8_t secret[CHALLENGE_SIZE];
    bool initialized;
} BoxContext;

/* create ACLs for secret data section */
BOX_CHALLENGE_ACL(g_box_acl);

/* configure secure box compartment and reserve box context */
UVISOR_BOX_CONFIG(box_challenge, g_box_acl, UVISOR_BOX_STACK_SIZE, BoxContext);

void *g_box_context;

static void randomize_new_secret(void)
{
    /* FIXME replace with HW-RNG */
    memset(uvisor_ctx->secret, 1, sizeof(uvisor_ctx->secret));

    /* the address of the context is stored and exposed, only for testing
     * purposes; uvisor protection ensures that accesses to this memory location
     * outside the box secure context will fail */
    g_box_context = (void *) uvisor_ctx;
}

static bool secure_compare(const uint8_t *src, const uint8_t *dst, int len)
{
    int diff;

    /* disallow length<0 */
    if(len<0)
        return false;

    /* time-constant comparison using XOR */
    diff = 0;
    while(len--)
        diff += *src++ ^ *dst++;

    /* if all bytes in src and dst are equal, the sum of the XOR's is zero */
    return !diff;
}

UVISOR_EXTERN bool __verify_secret(const uint8_t *secret, int len)
{
    /* We only accept calls where 'len' equals CHALLENGE_SIZE.
     * Although this creates a timing side channel which exposes the
     * CHALLENGE_SIZE to an attacker, we accept that side channel
     * as CHALLENGE_SIZE is exposed to the caller anyway as part of
     * the box headers.
     *
     * It's primarily a safety feature to ensure that caller and callee
     * share the same view of CHALLENGE_SIZE during runtime. The
     * security impact on this example is negligible.
     */
    if(len!=CHALLENGE_SIZE)
        return false;

    /* FIXME verify that secret pointer points outside of box stack context */

    /* Generate new secret on the first run. */
    /* FIXME enable clocks for HW-RNG */
    if (!uvisor_ctx->initialized) {
        randomize_new_secret();
        uvisor_ctx->initialized = true;
    }

    return secure_compare(secret, uvisor_ctx->secret, len);
}

bool verify_secret(const uint8_t *secret, int len)
{
    /* security transition happens here
     *   ensures that __verify_secret() will run with the privileges
     *   of the box_challenge box */
    return secure_gateway(box_challenge, __verify_secret, secret, len);
}
