/*
 *  Author: Kaka Xie
 *  brief: lock task
 */

#include "battery_task.h"
#include "battery.h"
#include "delay.h"

OS_STK BATTERY_TASK_STK[BATTERY_TASK_STK_SIZE];


uint16_t battery_voltage = 0;
#define BATTERY_VOLTAGE_BUF_SIZE    50
uint16_t battery_voltage_buffer[BATTERY_VOLTAGE_BUF_SIZE] = {0};
void battery_task(void *pdata)
{
    uint32_t cnt = 0;
    uint32_t voltage_sum = 0;
    while(1)
    {
        battery_voltage_buffer[cnt++ % BATTERY_VOLTAGE_BUF_SIZE] =  get_battery_voltage();
        voltage_sum = 0;
        for(uint8_t i = 0; i < BATTERY_VOLTAGE_BUF_SIZE; i++)
        {
            voltage_sum += battery_voltage_buffer[i];
        }
        battery_voltage = voltage_sum / BATTERY_VOLTAGE_BUF_SIZE;
        delay_ms(100);
    }
}

