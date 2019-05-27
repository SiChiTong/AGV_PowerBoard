/*
 *  Author: Kaka Xie
 *  brief: serial leds control
*/


#include "serial_led_task.h"
#include "serial_led.h"
#include "rgb_leds.h"
#include "delay.h"

OS_STK serial_led_task_stk[SERIAL_LED_TASK_STK_SIZE] = {0};

void serial_led_task(void *pdata)
{
    delay_ms(1000);
    //set_serial_leds_effect(LIGHTS_MODE_NORMAL, NULL, 0);
    set_rgb_leds_effect(LIGHTS_MODE_NORMAL, NULL, 0);
    while(1)
    {
        //serial_leds_tick();
        rgb_leds_tick();
        delay_ms(110);
    }
}

