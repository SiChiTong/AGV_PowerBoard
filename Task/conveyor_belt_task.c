/*
 *  Author: Kaka Xie
 *  brief: conveyor belt task
 */

#include "conveyor_belt_task.h"
#include "delay.h"

OS_STK conveyor_belt_task_stk[CONVEYOR_BELT_TASK_STK_SIZE] = {0};
extern void upload_conveyor_belt_status(uint8_t status);
void conveyor_belt_task(void *pdata)
{
    while(1)
    {
        delay_ms(1000);
        // test code
        //upload_conveyor_belt_status(2);
    }
}
