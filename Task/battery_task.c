/*
 *  Author: Kaka Xie
 *  brief: battery task
 */

#include "battery_task.h"
#include "battery.h"
#include "delay.h"
#include "platform.h"
#include "power_on_off_task.h"
#include "serial_led.h"
#include <stdio.h>

OS_STK BATTERY_TASK_STK[BATTERY_TASK_STK_SIZE];

void set_sys_low_power_power_off(void)
{
    post_power_on_off_signal();
    sys_status->sys_status |= STATE_IS_LOW_POWER;
}

void set_sys_low_power(void)
{
    set_serial_leds_effect(LIGHTS_MODE_LOW_POWER, NULL, 0);
}

void set_sys_normal_power(void)
{

}

#define LOW_POWER_WARNING_DEBAUNCE_TIME     15 * OS_TICKS_PER_SEC
static uint32_t low_power_warning_start_tick = 0;
void battery_low_power_process(void)
{
    uint8_t percentage = 0;
    if(battery_pack.com_status == TRUE)
    {
        percentage = battery_pack.percentage;
        if((sys_status->is_booting_up_finished == 1) && (sys_status->sys_status & STATE_POWER_ON))
        {
            if(((sys_status->sys_status & STATE_IS_CHARGER_IN) == 0) && ((sys_status->sys_status & STATE_IS_RECHARGE_IN) == 0))
            {
                if(percentage <= BATTERY_LOW_POWER_SHUTDOWN_PERCENTAGE)
                {
                    set_sys_low_power_power_off();
                }
                else if(percentage <= BATTERY_LOW_POWER_WARNING_PERCENTAGE)
                {
                    if(low_power_warning_start_tick == 0)
                    {
                        low_power_warning_start_tick = get_tick();
                    }
                    if(get_tick() - low_power_warning_start_tick >= LOW_POWER_WARNING_DEBAUNCE_TIME)
                    {
                        printf(" Low power ! ! \r\n Power is %d", percentage);
                        low_power_warning_start_tick = 0;
                        set_sys_low_power();
                    }
                }
                else
                {
                    set_sys_normal_power();
                }
            }
            else
            {
                /*
                    nothing to do
                */
            }
        }
    }
    else
    {
        /*
        TODO_MARK: can not get battery protocol info
        */
    }
}

void battery_task(void *pdata)
{
    delay_ms(500);
    battery_protocol_init();
    while(1)
    {
        delay_ms(5000);
        battery_period();
        battery_low_power_process();
    }
}

