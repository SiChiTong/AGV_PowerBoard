/*
 *  Author: Kaka Xie
 *  brief: led task
 */

#include "led_task.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "led.h"


OS_STK indicator_led_task_stk[INDICATOR_LED_STK_SIZE];

void indicator_led_task(void *pdata)
{
    delay_ms(500);
    while(1)
    {
        INDICATOR_LED = 0;
        delay_ms(500);
        INDICATOR_LED = 1;
        delay_ms(500);
    };
}

