/*
 *  Author: Kaka Xie
 *  brief: led task
 */

#include "led_task.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "led.h"

OS_STK LED1_TASK_STK[LED1_STK_SIZE] = {0};
OS_STK indicator_led_task_stk[INDICATOR_LED_STK_SIZE];

void led1_task(void *pdata)
{
    while(1)
    {
        LED0 = 1;
        delay_ms(80);
        LED0 = 0;
        delay_ms(920);
    };
}

////// test code //////
//#include "can_protocol_task.h"
//#include <string.h>
//#include "platform.h"

void indicator_led_task(void *pdata)
{
    ////// test code //////
//    uint16_t i = 0;
//    can_buf_t can_buf;

    delay_ms(500);
    while(1)
    {
        ////// test code //////
//        if(i > 0)
//        {
//            i--;
//            memset(can_buf.data, 0, 64);
//            can_buf.id = 0x7654321;
//            can_buf.data_len = 60;
//            memset(can_buf.data, i % 255, 64);
//            send_can_msg(&can_buf);
//        }
//        else
//        {
//            i = 0;
//        }

//        ir_led_pwm_ctrl(i++);
        INDICATOR_LED = 0;
        delay_ms(500);
        INDICATOR_LED = 1;
        delay_ms(500);
    };
}

