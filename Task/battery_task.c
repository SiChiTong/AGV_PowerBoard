/*
 *  Author: Kaka Xie
 *  brief: battery task
 */

#include "battery_task.h"
#include "battery.h"
#include "delay.h"

OS_STK BATTERY_TASK_STK[BATTERY_TASK_STK_SIZE];

void battery_task(void *pdata)
{
    delay_ms(500);
    battery_protocol_init();
    while(1)
    {
        delay_ms(100);
        battery_period();
    }
}

