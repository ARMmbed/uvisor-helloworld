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

#ifndef __BOX_CHALLENGE_H__
#define __BOX_CHALLENGE_H__

#define CHALLENGE_SIZE 16

UVISOR_EXTERN bool verify_secret(const uint8_t* secret, int len);

#endif/*__BOX_CHALLENGE_H__*/
