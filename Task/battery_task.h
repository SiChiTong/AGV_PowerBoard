#ifndef __USER_BATTERY_TASK_H__
#define __USER_BATTERY_TASK_H__
#include "stm32f10x.h"
#include "ucos_ii.h"

#define BATTERY_TASK_STK_SIZE   256


extern OS_STK BATTERY_TASK_STK[BATTERY_TASK_STK_SIZE];

void battery_task(void *pdata);
extern uint16_t battery_voltage;
#endif
