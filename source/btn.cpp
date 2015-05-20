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
#include <mbed-hal-ksdk-mcu/TARGET_KSDK_CODE/drivers/clock/fsl_clock_manager.h>
#include <mbed-hal-ksdk-mcu/TARGET_KSDK_CODE/hal/gpio/fsl_gpio_hal.h>
#include <mbed-hal-ksdk-mcu/TARGET_KSDK_CODE/hal/port/fsl_port_hal.h>
#include <mbed-hal-ksdk-mcu/PortNames.h>
#include "btn.h"

DigitalOut blue_led(LED_BLUE);

static void btn_on_press(void)
{
    /* if security is broken, unsecure code can access the password */
    printf("The leaked password is: %s\n\r", g_password);

    /* this code will never be reached because of the uVisor protection of
     * g_password */
    blue_led = 0;
    PORT_HAL_ClearPortIntFlag(PORTC_BASE);
}

void btn_init(void)
{
    /* turn off LED */
    blue_led = 1;

    /* enable clock for PORTC */
    CLOCK_SYS_EnablePortClock(PortC);

    /* set PORTC pin 6 to GPIO as an input, with interrupt on falling edge */
    PORT_HAL_SetMuxMode(PORTC_BASE, 6, kPortMuxAsGpio);
    GPIO_HAL_SetPinDir(PTC_BASE, 6, kGpioDigitalInput);
    PORT_HAL_SetPinIntMode(PORTC_BASE, 6, kPortIntFallingEdge);

    /* clear interrupt status flag and set interrupt handler */
    PORT_HAL_ClearPortIntFlag(PORTC_BASE);
    uvisor_set_isr(PORTC_IRQn, (uint32_t) &btn_on_press, 0);
    uvisor_enable_irq(PORTC_IRQn);
}
