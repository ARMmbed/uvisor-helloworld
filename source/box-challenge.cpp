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
#include "uvisor-lib/uvisor-lib.h"
#include "box-challenge.h"
#include "box-challenge-hw.h"

typedef struct {
    uint8_t secret[CHALLENGE_SIZE];
    bool initialized;
} BoxContext;

/* Create ACLs for the secret data section. */
BOX_CHALLENGE_ACL(g_box_acl);

/* Configure the secure box compartment and reserve box context. */
UVISOR_BOX_NAMESPACE(NULL);
UVISOR_BOX_CONFIG(box_challenge, g_box_acl, UVISOR_BOX_STACK_SIZE, BoxContext);

void *g_box_context;

static void randomize_new_secret(void)
{
    /* FIXME Replace with the use of a HW-RNG. */
    memset(uvisor_ctx->secret, 1, sizeof(uvisor_ctx->secret));
}

static bool secure_compare(const uint8_t *src, const uint8_t *dst, int len)
{
    int diff;

    /* Disallow a length less than 0. */
    if(len<0)
        return false;

    /* Perform a time-constant comparison using XOR. */
    diff = 0;
    while(len--)
        diff |= *src++ ^ *dst++;

    /* If all bytes in src and dst are equal, the sum of the XORs is zero. */
    return !diff;
}

static inline bool buffer_overlaps_box_context(const void *buffer, size_t length)
{
    uintptr_t buffer_addr = (uintptr_t) buffer;
    uintptr_t ctx_addr = (uintptr_t) uvisor_ctx;
    size_t ctx_length = sizeof(box_challenge_reserved);

    return (buffer_addr <= ctx_addr &&
            buffer_addr + length > ctx_addr) ||
           (ctx_addr <= buffer_addr &&
            ctx_addr + ctx_length > buffer_addr);
}

UVISOR_EXTERN bool __challenge_verify(const uint8_t *secret, int len)
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

    /* Verify that the secret pointer points outside of box stack context. This
     * is important to ensure that a challenger can't use memory internal to
     * the challenge box (stack, uvisor_ctx, etc.) to pass the challenge. */
    /* FIXME Use a uvisor-provided API to check that memory belongs to this box
     * or not. */
    if (buffer_overlaps_box_context(secret, len)) {
        return false;
    }

    /* Don't verify the challenge if the challenge hasn't been initialized. */
    if (!uvisor_ctx->initialized) {
        return false;
    }

    return secure_compare(secret, uvisor_ctx->secret, len);
}

bool challenge_verify(const uint8_t *secret, int len)
{
    /* A security transition happens here. This ensures that
     * __challenge_verify() will run with the privileges of the box_challenge
     * box. */
    return secure_gateway(box_challenge, __challenge_verify, secret, len);
}

UVISOR_EXTERN void __challenge_init(void)
{
    /* the address of the context is stored and exposed, only for testing
     * purposes; uvisor protection ensures that accesses to this memory location
     * outside the box secure context will fail */
    g_box_context = (void *) uvisor_ctx;

    /* FIXME enable clocks for HW-RNG */
    randomize_new_secret();

    uvisor_ctx->initialized = true;
}

void challenge_init(void)
{
    /* security transition happens here
     *   ensures that __secret_init() will run with the privileges
     *   of the box_challenge box */
    secure_gateway(box_challenge, __challenge_init);
}
