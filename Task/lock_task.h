#ifndef __USER_LOCK_TASK__
#define __USER_LOCK_TASK__
#include "stm32f10x.h"
#include "ucos_ii.h"


#define UNLOCK_TASK_STK_SIZE               256

extern OS_STK UNLOCK_TASK_STK[UNLOCK_TASK_STK_SIZE];




#define LOCK_STATUS_TASK_STK_SIZE               256

extern OS_STK LOCK_STATUS_TASK_STK[LOCK_STATUS_TASK_STK_SIZE];

extern OS_EVENT *unlock_sem;

void unlock_task(void *pdata);
void lock_status_task(void *pdata);
void start_to_unlock(void);

#endif
