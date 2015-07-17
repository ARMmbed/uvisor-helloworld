/***************************************************************
 * This confidential and  proprietary  software may be used only
 * as authorised  by  a licensing  agreement  from  ARM  Limited
 *
 *             (C) COPYRIGHT 2013-2014 ARM Limited
 *                      ALL RIGHTS RESERVED
 *
 *  The entire notice above must be reproduced on all authorised
 *  copies and copies  may only be made to the  extent permitted
 *  by a licensing agreement from ARM Limited.
 *
 ***************************************************************/
#include <mbed/mbed.h>
#include <uvisor-lib/uvisor-lib.h>
#include "box-challenge.h"

#define CHALLENGE_SIZE 16

typedef struct {
	bool initialized;
	uint8_t secret[CHALLENGE_SIZE];
} MyContext;

/* create ACLs for secret data section */
static const UvisorBoxAclItem g_box_acl[] = {
};

/* configure secure box compartnent and reserve box context */
UVISOR_BOX_CONTEXT(box_challenge, g_box_acl, UVISOR_BOX_STACK_SIZE, sizeof(MyContext));

/* all boxes share the same context pointer in public RAM
 * uVisor switches context to the box-specific context with
 * the specified length in UVISOR_BOX_STATE. The box context
 * is private per box */
#define g_box_state static_cast<MyContext*>(__uvisor_box_context)

static void __randomize_new_secret(void)
{
	/* fixme - replace with HW-RNG */
	memset(g_box_state->secret, 0, sizeof(g_box_state->secret));
}

static bool secure_compare(const uint8_t* src, const uint8_t* dst, int len)
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

UVISOR_EXTERN bool __verify_secret(const uint8_t* secret, int len)
{
	/* only accept the right challenge size */
	if(len!=CHALLENGE_SIZE)
		return false;

	/* generate new secret on the first run */
	if(!g_box_state->initialized)
		__randomize_new_secret();

	return secure_compare(secret, g_box_state->secret, len);
}

bool verify_secret(const uint8_t* secret, int len)
{
    /* security transition happens here
     *   ensures that __secure_print_pwd() will run with the privileges
     *   of the secure_print box - if uvisor-mode */
    if(__uvisor_mode)
        return secure_gateway(box_challenge, __verify_secret, secret, len);
    else
        /* fallback for disabled uvisor */
        return __verify_secret(secret, len);
}
