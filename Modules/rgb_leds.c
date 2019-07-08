/*
 *  Author: Kaka Xie
 *  brief: rgb leds module
 */


#include "rgb_leds.h"
#include "platform.h"
#include <string.h>

#if SERIAL_LED_TYPE == SERIAL_LED_RGB

static rgb_leds_mode_param_t cur_rgb_leds_effect = {0};

#define RGB_LIGHTNESS_PERCENT   30
const color_t rgb_color[] =
{
    [SERIAL_LED_COLOR_RED_C]       = {255 * RGB_LIGHTNESS_PERCENT / 100,     0 * RGB_LIGHTNESS_PERCENT / 100  ,     0 * RGB_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_GREEN_C]     = {0 * RGB_LIGHTNESS_PERCENT / 100  ,    200 * RGB_LIGHTNESS_PERCENT / 100,      0 * RGB_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_BLUE_C]      = {0 * RGB_LIGHTNESS_PERCENT / 100  ,    0 * RGB_LIGHTNESS_PERCENT / 100  ,      255 * RGB_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_ORANGE_C]    = {0xc8 * RGB_LIGHTNESS_PERCENT / 100,   0x32 * RGB_LIGHTNESS_PERCENT / 100,     0x00 * RGB_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_WHITE_C]     = {255 * RGB_LIGHTNESS_PERCENT / 100,    255 * RGB_LIGHTNESS_PERCENT / 100,      255 * RGB_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_CYAN_C]      = {0 * RGB_LIGHTNESS_PERCENT / 100  ,    255 * RGB_LIGHTNESS_PERCENT / 100,      255 * RGB_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_GOLD_C]      = {255 * RGB_LIGHTNESS_PERCENT / 100,    215 * RGB_LIGHTNESS_PERCENT / 100,      0 * RGB_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_SETTING_C]   = {0 * RGB_LIGHTNESS_PERCENT / 100  ,    0 * RGB_LIGHTNESS_PERCENT / 100  ,      0 * RGB_LIGHTNESS_PERCENT / 100  },
    [SERIAL_LED_COLOR_NONE_C]      = {0 * RGB_LIGHTNESS_PERCENT / 100  ,    0 * RGB_LIGHTNESS_PERCENT / 100  ,      0 * RGB_LIGHTNESS_PERCENT / 100  },
};


void set_rgb_color(const color_t color)
{
    set_rgb_leds_r(color.r);
    set_rgb_leds_g(color.g);
    set_rgb_leds_b(color.b);
}


#define SHINE_HIGH_SPEED_PERIOD         3 * OS_TICKS_PER_SEC / 10
#define SHINE_MEDIUM_SPEED_PERIOD       6 * OS_TICKS_PER_SEC / 10
#define SHINE_LOW_SPEED_PERIOD          1 * OS_TICKS_PER_SEC
#define UPDATE_PERIOD                   1 * OS_TICKS_PER_SEC
void set_rgb_leds_effect(const light_mode_t light_mode, color_t  *cur_color, const uint8_t period)
{
    static  light_mode_t pre_mode = LIGHTS_MODE_NONE;
    static  color_t      pre_color[2];
    static  uint8_t      pre_period;
    if((light_mode == pre_mode) && (light_mode != LIGHTS_MODE_SETTING))
    {
        return;
    }

    if(light_mode == LIGHTS_MODE_SETTING)
    {
        if((pre_color[0].b == cur_color[0].b) && (pre_color[0].g == cur_color[0].g) && (pre_color[0].r == cur_color[0].r))
        {
            if((pre_color[1].b == cur_color[1].b) && (pre_color[1].g == cur_color[1].g) && (pre_color[1].r == cur_color[1].r))
            {
                if(pre_period == period)
                {
                    return;
                }
            }
        }
    }

    pre_mode = light_mode;
    memcpy(&pre_color[0], &cur_color[0], sizeof(color_t) * 2);
//    memcpy(&pre_color[1], &cur_color[1], sizeof(color_t));
    pre_period = period;

    switch(light_mode)
    {
        case LIGHTS_MODE_NORMAL:
            cur_rgb_leds_effect.color[0] = rgb_color[SERIAL_LED_COLOR_ORANGE_C];
            cur_rgb_leds_effect.color[1] = rgb_color[SERIAL_LED_COLOR_NONE_C];
            cur_rgb_leds_effect.color_number = 2;
            cur_rgb_leds_effect.period = SHINE_MEDIUM_SPEED_PERIOD;
            cur_rgb_leds_effect.tick = 0;
            break;

        case LIGHTS_MODE_ERROR:
            cur_rgb_leds_effect.color[0] = rgb_color[SERIAL_LED_COLOR_RED_C];
            cur_rgb_leds_effect.color[1] = rgb_color[SERIAL_LED_COLOR_NONE_C];
            cur_rgb_leds_effect.color_number = 2;
            cur_rgb_leds_effect.period = SHINE_MEDIUM_SPEED_PERIOD;
            cur_rgb_leds_effect.tick = 0;
            break;

        case LIGHTS_MODE_COM_ERROR:
            cur_rgb_leds_effect.color[0] = rgb_color[SERIAL_LED_COLOR_RED_C];
            cur_rgb_leds_effect.color[1] = rgb_color[SERIAL_LED_COLOR_NONE_C];
            cur_rgb_leds_effect.color_number = 2;
            cur_rgb_leds_effect.period = SHINE_MEDIUM_SPEED_PERIOD;
            cur_rgb_leds_effect.tick = 0;
            break;

        case LIGHTS_MODE_LOW_POWER:
            cur_rgb_leds_effect.color[0] = rgb_color[SERIAL_LED_COLOR_RED_C];
            cur_rgb_leds_effect.color[1] = rgb_color[SERIAL_LED_COLOR_NONE_C];
            cur_rgb_leds_effect.color_number = 2;
            cur_rgb_leds_effect.period = SHINE_HIGH_SPEED_PERIOD;
            cur_rgb_leds_effect.tick = 0;
            break;

        case LIGHTS_MODE_CHARGING_POWER_LOW:
            cur_rgb_leds_effect.color[0] = rgb_color[SERIAL_LED_COLOR_RED_C];
            cur_rgb_leds_effect.color[1] = rgb_color[SERIAL_LED_COLOR_ORANGE_C];
            cur_rgb_leds_effect.color_number = 2;
            cur_rgb_leds_effect.period = SHINE_LOW_SPEED_PERIOD;
            cur_rgb_leds_effect.tick = 0;
            break;

        case LIGHTS_MODE_CHARGING_POWER_MEDIUM:
            cur_rgb_leds_effect.color[0] = rgb_color[SERIAL_LED_COLOR_ORANGE_C];
            cur_rgb_leds_effect.color[1] = rgb_color[SERIAL_LED_COLOR_GREEN_C];
            cur_rgb_leds_effect.color_number = 2;
            cur_rgb_leds_effect.period = SHINE_LOW_SPEED_PERIOD;
            cur_rgb_leds_effect.tick = 0;
            break;

        case LIGHTS_MODE_CHARGING_FULL:
            cur_rgb_leds_effect.color[0] = rgb_color[SERIAL_LED_COLOR_GREEN_C];
            cur_rgb_leds_effect.color_number = 1;
            cur_rgb_leds_effect.period = UPDATE_PERIOD;
            cur_rgb_leds_effect.tick = 0;
            break;

        case LIGHTS_MODE_TURN_LEFT:
            cur_rgb_leds_effect.color[1] = rgb_color[SERIAL_LED_COLOR_NONE_C];
            cur_rgb_leds_effect.color_number = 2;
            cur_rgb_leds_effect.period = SHINE_MEDIUM_SPEED_PERIOD;
            cur_rgb_leds_effect.tick = 0;
            break;

        case LIGHTS_MODE_TURN_RIGHT:
            cur_rgb_leds_effect.color[1] = rgb_color[SERIAL_LED_COLOR_NONE_C];
            cur_rgb_leds_effect.color_number = 2;
            cur_rgb_leds_effect.period = SHINE_MEDIUM_SPEED_PERIOD;
            cur_rgb_leds_effect.tick = 0;
            break;

        case LIGHTS_MODE_EMERGENCY_STOP:
            cur_rgb_leds_effect.color[0] = rgb_color[SERIAL_LED_COLOR_RED_C];//rgb_color[WHITE_C];
            cur_rgb_leds_effect.color_number = 1;
            cur_rgb_leds_effect.period = UPDATE_PERIOD;
            cur_rgb_leds_effect.tick = 0;

            break;
        case LIGHTS_MODE_SETTING:
            if(cur_color)
            {
                cur_rgb_leds_effect.color[0] = cur_color[0];
                cur_rgb_leds_effect.color[1] = cur_color[1];
                cur_rgb_leds_effect.color_number = 2;
                cur_rgb_leds_effect.period = period * 10 * OS_TICKS_PER_SEC / 1000;
                cur_rgb_leds_effect.tick = 0;
            }
            break;

        default :
            break;
    }
}


void rgb_leds_tick(void)
{
    if(get_tick() - cur_rgb_leds_effect.start_tick >= cur_rgb_leds_effect.period)
    {
        cur_rgb_leds_effect.tick++;
        cur_rgb_leds_effect.start_tick = get_tick();

        if(cur_rgb_leds_effect.color_number <= 2)
        {
            set_rgb_color(cur_rgb_leds_effect.color[cur_rgb_leds_effect.tick % cur_rgb_leds_effect.color_number]);
        }
    }
}

#endif
