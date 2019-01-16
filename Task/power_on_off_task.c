/*
 *  Author: Kaka Xie
 *  brief: power task
 */

#include "power_on_off_task.h"
#include "platform.h"
#include "delay.h"
#include "switch_task.h"


OS_STK power_on_off_stk[POWER_ON_OFF_STK_SIZE];
OS_STK x86_power_on_off_stk[X86_POWER_ON_OFF_STK_SIZE];
OS_STK rk_power_on_off_stk[RK_POWER_ON_OFF_STK_SIZE];

OS_EVENT *x86_power_on_sem;
OS_EVENT *x86_power_off_sem;

OS_EVENT *rk_power_on_sem;
OS_EVENT *rk_power_off_sem;

OS_EVENT *power_on_sem;
OS_EVENT *power_off_sem;


void power_on_off_x86_task(void *pdata)
{
    uint8_t err = 0;
    while(1)
    {
        OSSemPend(x86_power_on_sem, 0, &err);
        delay_ms(X86_POWER_ON_DELAY_TIME);
        x86_power_signal_ctrl(1);
        delay_ms(X86_POWER_ON_HOLD_TIME);
        x86_power_signal_ctrl(0);

        OSSemPend(x86_power_off_sem, 0, &err);
        x86_power_signal_ctrl(1);
        delay_ms(X86_POWER_OFF_HOLD_TIME);
        x86_power_signal_ctrl(0);
    }
}

void power_on_off_rk_task(void *pdata)
{
    uint8_t err = 0;
    while(1)
    {
        OSSemPend(rk_power_on_sem, 0, &err);
        delay_ms(RK_POWER_ON_DELAY_TIME);
        rk_power_signal_ctrl(1);
        delay_ms(RK_POWER_ON_HOLD_TIME);
        rk_power_signal_ctrl(0);

        OSSemPend(rk_power_off_sem, 0, &err);
        rk_power_signal_ctrl(1);
        delay_ms(RK_POWER_OFF_HOLD_TIME);
        rk_power_signal_ctrl(0);
    }
}

void power_on_off_task(void *pdata)
{
    uint8_t err = 0;
    while(1)
    {
        /*boot up*/
//        OSSemPend(power_on_sem, 0, &err);
        hold_on_power();
        beeper_on();
        main_power_module_5v_ctrl(MODULE_POWER_ON);
        main_power_module_12v_ctrl(MODULE_POWER_ON);
        main_power_module_24v_ctrl(MODULE_POWER_ON);
        delay_ms(500);
        beeper_off();
        OSSemPost(x86_power_on_sem);
        OSSemPost(rk_power_on_sem);
        delay_ms(BOOTING_UP_TIME);

        /*shutdown*/
        powerkey_long_press_sem = OSSemCreate(0);
        OSSemPend(powerkey_long_press_sem, 0, &err);
        beeper_on();
        delay_ms(500);
        beeper_off();
        delay_ms(1000);
        OSSemPost(x86_power_off_sem);
        OSSemPost(rk_power_off_sem);
        delay_ms(SHUTTING_DOWN_TIME);
        main_power_module_5v_ctrl(MODULE_POWER_OFF);
        main_power_module_12v_ctrl(MODULE_POWER_OFF);
        main_power_module_24v_ctrl(MODULE_POWER_OFF);
        release_power();
        delay_ms(2000);
    }
}


