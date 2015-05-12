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
#ifndef __BOX_SECURE_PRINT_H__
#define __BOX_SECURE_PRINT_H__

extern volatile int g_polling;

void secure_timer_init(void);
void secure_print_msg(char *buffer, int len);
void secure_print_pwd(void);

#endif/*__BOX_SECURE_PRINT_H__*/
