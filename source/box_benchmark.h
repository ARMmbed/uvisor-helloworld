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
#ifndef __BOX_ENCHMARK_H__
#define __BOX_ENCHMARK_H__

typedef enum {
    secure = 0,
    unsecure
} TBenchmarkMode;

uint32_t benchmark_mock(TBenchmarkMode mode);

#endif/*__BOX_BENCHMARK_H__*/
