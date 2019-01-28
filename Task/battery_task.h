#ifndef __USER_BATTERY_TASK_H__
#define __USER_BATTERY_TASK_H__
#include "stm32f10x.h"
#include "ucos_ii.h"
#include "sys.h"

#define BATTERY_TASK_STK_SIZE   512


#define BATTERY_LOW_POWER_SHUTDOWN_PERCENTAGE       10
#define BATTERY_LOW_POWER_WARNING_PERCENTAGE        20


extern OS_STK battery_task_stk[BATTERY_TASK_STK_SIZE];

void battery_task(void *pdata);
extern uint16_t battery_voltage;
#endif
