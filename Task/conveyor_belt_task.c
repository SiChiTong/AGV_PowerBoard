/*
 *  Author: Kaka Xie
 *  brief: conveyor belt task
 */
#include "conveyor_belt_task.h"
#include "delay.h"

OS_STK conveyor_belt_task_stk[CONVEYOR_BELT_TASK_STK_SIZE] = {0};

void conveyor_belt_task(void *pdata)
{
    while(1)
    {
        delay_ms(1000);
    }
}
