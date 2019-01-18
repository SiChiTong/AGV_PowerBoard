#ifndef __TASK_CHARGE_TASK_H__
#define __TASK_CHARGE_TASK_H__

#include "stm32f10x.h"
#include "ucos_ii.h"

#define CHARGE_TASK_STK_SIZE    256
extern OS_STK charge_task_stk[CHARGE_TASK_STK_SIZE];
extern OS_EVENT * charge_state_mailbox;
void charge_task(void *pdata);
#endif
