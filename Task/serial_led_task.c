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
#if SERIAL_LED_TYPE == SERIAL_LED_ONE_WIRE
    set_serial_leds_effect(LIGHTS_MODE_NORMAL, NULL, 0);
#elif SERIAL_LED_TYPE == SERIAL_LED_RGB
    set_rgb_leds_effect(LIGHTS_MODE_NORMAL, NULL, 0);
#endif
    while(1)
    {
#if SERIAL_LED_TYPE == SERIAL_LED_ONE_WIRE
        serial_leds_tick();
#elif SERIAL_LED_TYPE == SERIAL_LED_RGB
        rgb_leds_tick();
#endif
        delay_ms(110);
    }
}

