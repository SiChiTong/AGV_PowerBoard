/*
 *  Author: Kaka Xie
 *  brief: switch task
*/
#include "charge_task.h"
#include "delay.h"


OS_STK charge_task_stk[CHARGE_TASK_STK_SIZE];

void charge_task(void *pdata)
{
    while(1)
    {
        delay_ms(1000);
    }
}
