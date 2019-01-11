/*
 *  Author: Kaka Xie
 *  brief: platform
 */

#include "platform.h"
#include "stdio.h"
#include "battery.h"
#include "led.h"
#include "can.h"

uint32_t get_tick(void)
{
    return OSTimeGet();
}


void hardware_init(void)
{
    battery_adc_init();
    led_init();
    CanInit();
}


