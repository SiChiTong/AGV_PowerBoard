/*
 *  Author: Kaka Xie
 *  brief: switch task
*/
#include "switch_task.h"
#include "platform.h"
#include "delay.h"

OS_STK switch_task_stk[SWITCH_TASK_STK_SIZE] = {0};

OS_EVENT *powerkey_long_press_sem;

extern void upload_event_button_state(void);
#define SWITCH_LONG_PRESS_TICK      1 * OS_TICKS_PER_SEC
#define SWITCH_SHORT_PRESS_TICK     (5 * OS_TICKS_PER_SEC / 100)
void switch_task(void *pdata)
{
    uint8_t power_but_cur_state = 0;
    uint8_t power_but_pre_state = 0;
    uint32_t power_but_start_tick = get_tick();
    uint8_t event_but_cur_state = 0;
    uint8_t event_but_pre_state = 0;
    uint32_t event_but_start_tick = get_tick();
    uint8_t state = 0;
    while(1)
    {
        power_but_cur_state = get_switch_state();
        if((power_but_cur_state == power_but_pre_state) && (power_but_cur_state == 1))
        {
            if(get_tick() - power_but_start_tick >= SWITCH_LONG_PRESS_TICK)
            {
                OSSemPost(powerkey_long_press_sem);
            }
        }
        else
        {
            power_but_start_tick = get_tick();
        }
        power_but_pre_state = power_but_cur_state;



//        event_but_cur_state = get_event_buttton_state();
//        if((event_but_cur_state == event_but_pre_state) && (event_but_cur_state == 0))
//        {
//            if(get_tick() - event_but_start_tick >= SWITCH_SHORT_PRESS_TICK)
//            {
//                sys_status->event_button_state = event_but_cur_state;
//                upload_event_button_state();
//            }
//        }
//        else
//        {
//            event_but_start_tick = get_tick();
//        }
//        event_but_pre_state = event_but_cur_state;
        event_but_cur_state = get_event_buttton_state();
        switch(state)
        {
            case 0:
                if((event_but_cur_state == event_but_pre_state) && (event_but_cur_state == 0))
                {
                    state = 1;
                    event_but_start_tick = get_tick();
                }
                else
                {
                    break;
                }

            case 1:
                if((event_but_cur_state != event_but_pre_state) || (event_but_cur_state != 0))
                {
                    state = 0;
                    break;
                }

                if(get_tick() - event_but_start_tick >= SWITCH_SHORT_PRESS_TICK)
                {
                    sys_status->event_button_state = event_but_cur_state;
                    upload_event_button_state();
                    state = 2;
                }
                else
                {
                    break;
                }

            case 2:
                //beeper_on();
                //delay_ms(100);
                //beeper_off();
                state = 3;
                break;

            case 3:
                if(event_but_cur_state == 1)
                {
                    state = 0;
                }
                break;

            default :
                break;
        }
        event_but_pre_state = event_but_cur_state;

        delay_ms(50);
    }
}
