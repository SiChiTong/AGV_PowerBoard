#ifndef __TASK_SWITCH_TASK_H__
#define __TASK_SWITCH_TASK_H__

#include "stm32f10x.h"
#include "ucos_ii.h"

#define SWITCH_TASK_STK_SIZE    64

extern OS_STK switch_task_stk[SWITCH_TASK_STK_SIZE];

extern OS_EVENT *powerkey_long_press_sem;

void switch_task(void *pdata);
#endif
