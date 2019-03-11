#ifndef __TASK_POWER_ON_OFF_TASK__H__
#define __TASK_POWER_ON_OFF_TASK__H__
#include "stm32f10x.h"
#include "ucos_ii.h"

#define POWER_ON_OFF_STK_SIZE           256
#define X86_POWER_ON_OFF_STK_SIZE       128
#define RK_POWER_ON_OFF_STK_SIZE        128

extern OS_STK power_on_off_stk[POWER_ON_OFF_STK_SIZE];
extern OS_STK x86_power_on_off_stk[X86_POWER_ON_OFF_STK_SIZE];
extern OS_STK rk_power_on_off_stk[RK_POWER_ON_OFF_STK_SIZE];



extern OS_EVENT *power_on_sem;
extern OS_EVENT *power_off_sem;
extern OS_EVENT *x86_power_on_sem;
extern OS_EVENT *x86_power_off_sem;
extern OS_EVENT *rk_power_on_sem;
extern OS_EVENT *rk_power_off_sem;

//#define BOOTING_UP_TIME         45*1000
//#define SHUTTING_DOWN_TIME      30*1000

#define BOOTING_UP_TIME         45*1000
#define SHUTTING_DOWN_TIME      30*1000

#define X86_POWER_ON_DELAY_TIME         3500
#define X86_POWER_OFF_DELAY_TIME        1000
#define X86_POWER_ON_HOLD_TIME          2000
#define X86_POWER_OFF_HOLD_TIME         500
#define X86_POWER_ON_PROCESSING_TIME    15000
#define X86_POWER_OFF_PROCESSING_TIME   15000


#define RK_POWER_ON_DELAY_TIME          3200
#define RK_POWER_OFF_DELAY_TIME         1000
#define RK_POWER_ON_HOLD_TIME           4000
#define RK_POWER_OFF_HOLD_TIME          5000
#define RK_POWER_ON_PROCESSING_TIME     30000
#define RK_POWER_OFF_PROCESSING_TIME    20000


void power_on_off_task(void *pdata);
void power_on_off_x86_task(void *pdata);
void power_on_off_rk_task(void *pdata);
void post_power_on_off_signal(void);
#endif

