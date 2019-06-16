#ifndef __TASK_POWER_ON_SELF_TEST_H__
#define __TASK_POWER_ON_SELF_TEST_H__
#include "stm32f10x.h"
#include "ucos_ii.h"

#define POWER_ON_SELF_TEST_STK_SIZE     64


extern OS_STK POST_task_stk[POWER_ON_SELF_TEST_STK_SIZE];

extern OS_EVENT *start_POST_sem;

void POST_task(void *pdata);

#endif

