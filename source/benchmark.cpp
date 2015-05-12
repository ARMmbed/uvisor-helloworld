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
#include "mbed-hal-ksdk-mcu/TARGET_KSDK_CODE/hal/pit/fsl_pit_hal.h"
#include "mbed-hal-ksdk-mcu/TARGET_KSDK_CODE/drivers/clock/fsl_clock_manager.h"
#include "benchmark.h"

#define BENCHMARK_TIMER_LDVAL 0xFFFFFFFFU

volatile int g_benchmark_overflow;

/* create ACLs for secret data section */
static const UvisorBoxAclItem g_box_acl[] = {
};

/* configure secure box compartnent */
UVISOR_BOX_CONFIG(benchmark_box, g_box_acl, UVISOR_BOX_STACK_SIZE);

static void benchmark_timer_handler(void)
{
    /* we assume only small intervals are measured */
    g_benchmark_overflow = 1;

    /* stop timer and clear interrupt flag */
    PIT_HAL_StopTimer(PIT_BASE, 3);
    PIT_HAL_ClearIntFlag(PIT_BASE, 3);
}

static void benchmark_timer_start(void)
{
    /* enable interrupts for timer */
    PIT_HAL_SetIntCmd(PIT_BASE, 3, true);

    /* start timer */
    PIT_HAL_StartTimer(PIT_BASE, 3);
}

static uint32_t benchmark_timer_stop()
{
    uint64_t pit_src_clk;
    uint32_t stop, elapsed, elapsed_ns;

    /* freeze timer temporarily to get value */
    stop = PIT_HAL_ReadTimerCount(PIT_BASE, 3);
    PIT_HAL_StopTimer(PIT_BASE, 3);

    /* convert to us for print */
    if(g_benchmark_overflow)
    {
        elapsed = 0;
        elapsed_ns = 0;
        g_benchmark_overflow = 0;
    }
    else
    {
        pit_src_clk  = CLOCK_SYS_GetPitFreq(0);
        elapsed = BENCHMARK_TIMER_LDVAL - stop;
        elapsed_ns = (elapsed + 1) * (1000000000 / pit_src_clk);
    }

    /* disable interrupts for timer */
    PIT_HAL_SetIntCmd(PIT_BASE, 3, false);

    return elapsed_ns;
}

static void benchmark_timer_init(void)
{
    /* enable clock for PIT module */
    CLOCK_SYS_EnablePitClock(0);

    /* turn on the PIT module (freeze during debug) */
    PIT_HAL_Enable(PIT_BASE);
    PIT_HAL_SetTimerRunInDebugCmd(PIT_BASE, false);

    /* configure interrupt */
    uvisor_set_isr(PIT3_IRQn, (uint32_t) &benchmark_timer_handler, 0);
    uvisor_enable_irq(PIT3_IRQn);

    /* configure registers */
    PIT_HAL_SetTimerPeriodByCount(PIT_BASE, 3, BENCHMARK_TIMER_LDVAL);
}

UVISOR_EXTERN uint32_t __benchmark_mock(void)
{
    return 0;
}

uint32_t benchmark_mock(TBenchmarkMode mode)
{
    /* initialize the timer */
    benchmark_timer_init();

    /* start measuring */
    benchmark_timer_start();

    /* perform function call */
    if(__uvisor_mode && (mode == secure))
        secure_gateway(benchmark_box, __benchmark_mock);
    else
        /* fallback for disabled uvisor */
        __benchmark_mock();

    /* return measured execution time */
    return benchmark_timer_stop();
}
