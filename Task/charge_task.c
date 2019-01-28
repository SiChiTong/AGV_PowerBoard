/*
 *  Author: Kaka Xie
 *  brief: charge task
*/
#include "charge_task.h"
#include "delay.h"
#include "can_protocol_task.h"
#include "platform.h"


OS_STK charge_task_stk[CHARGE_TASK_STK_SIZE] = {0};
OS_EVENT * charge_state_mailbox;


void charge_task(void *pdata)
{
    uint8_t err;
    uint32_t charge_recharge_state = 0;
    uint8_t charge_state = 0;
    uint8_t recharge_state = 0;
//    uint32_t charge_recharge_state_tmp = 0;
    uint8_t cnt = 0;
    uint32_t pre_charge_recharge_state = 0x1234;
    while(1)
    {
        charge_recharge_state = (uint32_t)OSMboxPend(charge_state_mailbox, 0, &err);// 此处的邮箱可以更改为信号量
        recharge_state = get_recharge_gpio_value();
        pre_charge_recharge_state &= 0xffffff00;
        pre_charge_recharge_state += recharge_state;

        charge_state = get_charge_gpio_value();
        pre_charge_recharge_state &= 0xffff00ff;
        pre_charge_recharge_state += charge_state << 8;
        while(cnt <= 2)
        {
            delay_ms(10);
            recharge_state = get_recharge_gpio_value();
            charge_recharge_state &= 0xffffff00;
            charge_recharge_state += recharge_state;

            charge_state = get_charge_gpio_value();
            charge_recharge_state &= 0xffff00ff;
            charge_recharge_state += charge_state << 8;
            if(pre_charge_recharge_state == charge_recharge_state)
            {
                cnt++;
            }
            else
            {
                cnt = 0;
            }
            pre_charge_recharge_state = charge_recharge_state;
        }
        cnt = 0;

        if(charge_state)
        {
            sys_status->sys_status |= STATE_IS_CHARGER_IN;
        }
        else
        {
            sys_status->sys_status &= ~STATE_IS_CHARGER_IN;
        }
        if(recharge_state)
        {
            sys_status->sys_status |= STATE_IS_RECHARGE_IN;
        }
        else
        {
            sys_status->sys_status &= ~STATE_IS_RECHARGE_IN;
        }
        upload_sys_state();
        delay_ms(20);
    }
}
