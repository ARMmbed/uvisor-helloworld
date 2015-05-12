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
#include <mbed.h>
#include <uvisor-lib/uvisor-lib.h>
#include "box_benchmark.h"

/* create ACLs for secret data section */
static const UvisorBoxAclItem g_box_acl[] = {
};

/* configure secure box compartnent */
UVISOR_BOX_CONFIG(benchmark_box, g_box_acl, UVISOR_BOX_STACK_SIZE);

UVISOR_EXTERN uint32_t __benchmark_mock(void)
{
    return 0;
}

uint32_t benchmark_mock(TBenchmarkMode mode)
{
    /* start measuring */
    uvisor_benchmark_start();

    /* perform function call */
    if(__uvisor_mode && (mode == secure))
        secure_gateway(benchmark_box, __benchmark_mock);
    else
        /* fallback for disabled uvisor */
        __benchmark_mock();

    /* return measured execution time */
    return uvisor_benchmark_stop();
}
