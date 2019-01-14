/*
 *  Author: Kaka Xie
 *  brief: power task
 */

#include "power_on_off_task.h"
#include "platform.h"
#include "delay.h"


OS_STK power_on_stk[POWER_ON_STK_SIZE];

void power_on_x86(void *pdata)
{
    
}

void power_on_task(void *pdata)
{
    hold_on_power();
    main_power_module_5v_ctrl(MODULE_POWER_ON);
    main_power_module_12v_ctrl(MODULE_POWER_ON);
    main_power_module_12v_ctrl(MODULE_POWER_ON);
    while(1)
    {
        delay_ms(1000);
    }
}

void power_off_task(void)
{
    
}

void power_task(void *pdata)
{
    
}
