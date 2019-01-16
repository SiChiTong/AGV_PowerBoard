/*
 *  Author: Kaka Xie
 *  brief: user configuration ucosii
 */

#include "common.h"

extern void falsh_test_task(void *pdata);

static void task_create(void)
{
    OSTaskCreate(indicator_led_task,        (void *)0,  (OS_STK*)&INDICATOR_LED_TASK_STK[INDICATOR_LED_STK_SIZE-1],                         INDICATOR_LED_TASK_PRIO);
    OSTaskCreate(battery_task,              (void *)0,  (OS_STK*)&BATTERY_TASK_STK[BATTERY_TASK_STK_SIZE - 1],                              BATTERY_TASK_PRIO);
    OSTaskCreate(can_protocol_task,         (void *)0,  (OS_STK*)&can_protocol_task_stk[CAN_PROTOCOL_TASK_STK_SIZE - 1],                    CAN_RPOTOCOL_TASK_PRIO);
    OSTaskCreate(power_on_off_task,         (void *)0,  (OS_STK*)&power_on_off_stk[POWER_ON_OFF_STK_SIZE - 1],                              POWER_ON_OFF_TASK_PRIO);
    OSTaskCreate(switch_task,               (void *)0,  (OS_STK*)&switch_task_stk[SWITCH_TASK_STK_SIZE - 1],                                SWITCH_TASK_PRIO);
    OSTaskCreate(power_on_off_x86_task,     (void *)0,  (OS_STK*)&x86_power_on_off_stk[X86_POWER_ON_OFF_STK_SIZE - 1],                      X86_POWER_ON_OFF_TASK_PRIO);
    OSTaskCreate(power_on_off_rk_task,      (void *)0,  (OS_STK*)&rk_power_on_off_stk[RK_POWER_ON_OFF_STK_SIZE - 1],                        RK_POWER_ON_OFF_TASK_PRIO);

}

static void sem_create(void)
{
    powerkey_long_press_sem = OSSemCreate(0);
    x86_power_on_sem = OSSemCreate(0);
    x86_power_off_sem = OSSemCreate(0);
    rk_power_on_sem = OSSemCreate(0);
    rk_power_off_sem = OSSemCreate(0);
    power_on_sem = OSSemCreate(0);
    power_off_sem = OSSemCreate(0);
}


static int mem_create(void)
{
    return 0;
}

static int queue_create(void)
{
    return 0;
}

static void os_user_config(void)
{
    sem_create();
    mem_create();
    queue_create();
    task_create();
}

static void user_init_depend_on_os_config(void)
{

}

void user_init(void)
{
    os_user_config();
    user_init_depend_on_os_config();
}

