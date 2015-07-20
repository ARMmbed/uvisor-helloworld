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
#include "mbed/mbed.h"
#include "uvisor-lib/uvisor-lib.h"
#include "box-challenge.h"
#include "box-challenge-acl.h"

typedef struct {
	bool initialized;
	uint8_t secret[CHALLENGE_SIZE];
} BoxContext;

/* create ACLs for secret data section */
BOX_CHALLENGE_ACL(g_box_acl);

/* configure secure box compartnent and reserve box context */
UVISOR_BOX_CONFIG(box_challenge, g_box_acl, UVISOR_BOX_STACK_SIZE, BoxContext);

static void __randomize_new_secret(void)
{
	/* FIXME replace with HW-RNG */
	memset(uvisor_box_context->secret, 0, sizeof(uvisor_box_context->secret));
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

UVISOR_EXTERN bool __verify_secret(const uint8_t* secret, int len)
{
	/* only accept the right challenge size */
	if(len!=CHALLENGE_SIZE)
		return false;

	/* FIXME verify that secret pointer points outside of box stack context */

	/* generate new secret on the first run
	 * FIXME enable clocks for HW-RNG */
	if(!uvisor_box_context->initialized)
		__randomize_new_secret();

	return secure_compare(secret, uvisor_box_context->secret, len);
}

bool verify_secret(const uint8_t* secret, int len)
{
    /* security transition happens here
     *   ensures that __verify_secret() will run with the privileges
     *   of the box_challenge box */
    return secure_gateway(box_challenge, __verify_secret, secret, len);
}
