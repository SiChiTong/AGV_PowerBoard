#ifndef __TASK_POWER_ON_OFF_TASK__H__
#define __TASK_POWER_ON_OFF_TASK__H__
#include "stm32f10x.h"
#include "ucos_ii.h"

#define POWER_ON_STK_SIZE   128

extern OS_STK power_on_stk[POWER_ON_STK_SIZE];

void power_on_task(void *pdata);
#endif

