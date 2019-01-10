#ifndef __USER_LED_TASK_H__
#define __USER_LED_TASK_H__
#include "stm32f10x.h"
#include "ucos_ii.h"



#define LED1_STK_SIZE                   64
extern OS_STK LED1_TASK_STK[LED1_STK_SIZE];



#define INDICATOR_LED_STK_SIZE                   64
extern OS_STK INDICATOR_LED_TASK_STK[INDICATOR_LED_STK_SIZE];


#define FLASH_TEST_TASK_PRIO                  11
#define FLASH_TEST_STK_SIZE                   64
extern OS_STK FLASH_TEST_TASK_STK[FLASH_TEST_STK_SIZE];

void led1_task(void *pdata);
void indicator_led_task(void *pdata);

#endif
