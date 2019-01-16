#ifndef __TASK_POWER_ON_OFF_TASK__H__
#define __TASK_POWER_ON_OFF_TASK__H__
#include "stm32f10x.h"
#include "ucos_ii.h"

#define POWER_ON_OFF_STK_SIZE           128
#define X86_POWER_ON_OFF_STK_SIZE       64
#define RK_POWER_ON_OFF_STK_SIZE        64

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

#define BOOTING_UP_TIME         15*1000
#define SHUTTING_DOWN_TIME      10*1000

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


#define SYS_POWER_STATE_BOOTING_UP      1
#define SYS_POWER_STATE_ON              2
#define SYS_POWER_STATE_SHUTTING_DOWN   3
#define SYS_POWER_STATE_OFF             0
typedef struct
{
    uint8_t power_state;
    uint32_t sys_state;
    uint8_t is_booting_up_finished;
    uint8_t is_shutting_down_finished;
}sys_power_t;

void power_on_off_task(void *pdata);
void power_on_off_x86_task(void *pdata);
void power_on_off_rk_task(void *pdata);
#endif

