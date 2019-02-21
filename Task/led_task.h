#ifndef __USER_LED_TASK_H__
#define __USER_LED_TASK_H__
#include "stm32f10x.h"
#include "ucos_ii.h"

#define INDICATOR_LED_STK_SIZE                   64
extern OS_STK indicator_led_task_stk[INDICATOR_LED_STK_SIZE];

void indicator_led_task(void *pdata);

#endif
