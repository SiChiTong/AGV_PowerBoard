#ifndef __CONVEYOR_BELT_H__
#define __CONVEYOR_BELT_H__
#include "stm32f10x.h"
#include "ucos_ii.h"
#define CONVEYOR_BELT_TASK_STK_SIZE    128

extern OS_STK conveyor_belt_task_stk[CONVEYOR_BELT_TASK_STK_SIZE];


void conveyor_belt_task(void *pdata);
#endif
