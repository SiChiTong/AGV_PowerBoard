#ifndef __MODULE_RGB_LEDS_H__
#define __MODULE_RGB_LEDS_H__

#include "stm32f10x.h"
#include "serial_led.h"

typedef struct
{
    uint16_t period;
    color_t color[5];
    uint16_t color_number;
    volatile uint32_t start_tick;
    volatile uint32_t tick;
}rgb_leds_mode_param_t;

void set_rgb_leds_effect(const light_mode_t light_mode, color_t  *cur_color, const uint8_t period);
void rgb_leds_tick(void);
#endif

