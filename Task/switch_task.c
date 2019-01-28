/*
 *  Author: Kaka Xie
 *  brief: switch task
*/
#include "switch_task.h"
#include "platform.h"
#include "delay.h"

OS_STK switch_task_stk[SWITCH_TASK_STK_SIZE] = {0};

OS_EVENT *powerkey_long_press_sem;

#define SWITCH_LONG_PRESS_TICK  1 * OS_TICKS_PER_SEC
void switch_task(void *pdata)
{
    uint8_t cur_state = 0;
    uint8_t pre_state = 0;
    uint32_t start_tick = get_tick();;
    while(1)
    {
        cur_state = get_switch_state();
        if((cur_state == pre_state) && (cur_state == 1))
        {
            if(get_tick() - start_tick >= SWITCH_LONG_PRESS_TICK)
            {
                OSSemPost(powerkey_long_press_sem);
            }
        }
        else
        {
            start_tick = get_tick();
        }
        pre_state = cur_state;
        delay_ms(100);
    }
}
