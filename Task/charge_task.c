/*
 *  Author: Kaka Xie
 *  brief: charge task
*/
#include "charge_task.h"
#include "delay.h"
#include "can_protocol_task.h"
#include "platform.h"


OS_STK charge_task_stk[CHARGE_TASK_STK_SIZE];
OS_EVENT * charge_state_mailbox;


void charge_task(void *pdata)
{
    uint8_t err;
    uint32_t charge_recharge_state = 0;
//    uint16_t pre_charge_recharge_state = 0;
    while(1)
    {
        charge_recharge_state = (uint32_t)OSMboxPend(charge_state_mailbox, 0, &err);
//        if(charge_recharge_state == pre_charge_recharge_state)
        {
            sys_status->charge_state = charge_recharge_state & 0xff;
            sys_status->recharge_state = (charge_recharge_state >> 8) & 0xff;
            if(sys_status->charge_state)
            {
                sys_status->sys_status |= STATE_IS_CHARGER_IN;
            }
            else
            {
                sys_status->sys_status &= ~STATE_IS_CHARGER_IN;
            }
            if(sys_status->recharge_state)
            {
                sys_status->sys_status |= STATE_IS_RECHARGE_IN;
            }
            else
            {
                sys_status->sys_status &= ~STATE_IS_RECHARGE_IN;
            }
            upload_sys_state();
        }
//        pre_charge_recharge_state = charge_recharge_state;
//        delay_ms(100);
    }
}
