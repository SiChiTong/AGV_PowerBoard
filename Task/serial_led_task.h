#ifndef __TASK_SERIAL_LED_H__
#define __TASK_SERIAL_LED_H__

#include "stm32f10x.h"
#include "ucos_ii.h"

#define SERIAL_LED_TASK_STK_SIZE    256

extern OS_STK serial_led_task_stk[SERIAL_LED_TASK_STK_SIZE];

void serial_led_task(void *pdata);


#endif
