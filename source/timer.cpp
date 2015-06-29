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
#include "mbed/mbed.h"
#include "mbed-hal-ksdk-mcu/TARGET_KSDK_CODE/hal/pit/fsl_pit_hal.h"
#include "mbed-hal-ksdk-mcu/TARGET_KSDK_CODE/drivers/clock/fsl_clock_manager.h"
#include "timer.h"

/* timer period */
#define TIMER_T_US 2000000U

/* polling variable */
volatile int g_timer_polling;

/* timer handler
 * since this handler is managed by the uVisor, it will not be executed in
 * handler mode, but in unprivileged thread mode; resources of other secure
 * boxes are not at risk! */
static void timer_handler(void)
{
    /* set polling variable */
    g_timer_polling = 1;

    /* clear interrupt flag */
    PIT_HAL_ClearIntFlag(PIT_BASE, 1);
}

/* initialize timer */
void timer_init(void)
{
    uint32_t cnt;

    /* enable clock for PIT module */
    CLOCK_SYS_EnablePitClock(0);

    /* turn on the PIT module (freeze during debug) */
    PIT_HAL_Enable(PIT_BASE);
    PIT_HAL_SetTimerRunInDebugCmd(PIT_BASE, false);

    /* configure interrupt */
    uvisor_isr_set(PIT1_IRQn, (uint32_t) &timer_handler, 0);
    uvisor_irq_enable(PIT1_IRQn);

    /* configure registers */
    cnt = (uint32_t) ((uint64_t) TIMER_T_US * CLOCK_SYS_GetPitFreq(0) /
                      1000000U - 1U);
    PIT_HAL_SetTimerPeriodByCount(PIT_BASE, 1, cnt);
    PIT_HAL_StartTimer(PIT_BASE, 1);

    /* enable interrupts */
    PIT_HAL_SetIntCmd(PIT_BASE, 1, true);
}
