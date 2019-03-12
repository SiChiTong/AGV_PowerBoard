/*
 *  Author: Kaka Xie
 *  brief: serial leds control
 */

#include "serial_led.h"
#include "delay.h"
#include <string.h>


__IO uint32_t front_left_buff[FRONT_LEFT_LED_NUM] = {0};
__IO uint32_t front_right_buff[FRONT_RIGHT_LED_NUM] = {0};
__IO uint32_t back_right_buff[BACK_RIGHT_LED_NUM] = {0};
__IO uint32_t back_left_buff[BACK_LEFT_LED_NUM] = {0};
__IO uint32_t eyes_buff[EYES_LED_NUM] = {0};


#define LED_LIGHTNESS_PERCENT   50
color_t led_color[] =
{
    [SERIAL_LED_COLOR_RED_C]       = {255 * LED_LIGHTNESS_PERCENT / 100,     0 * LED_LIGHTNESS_PERCENT / 100  ,     0 * LED_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_GREEN_C]     = {0 * LED_LIGHTNESS_PERCENT / 100  ,    200 * LED_LIGHTNESS_PERCENT / 100,      0 * LED_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_BLUE_C]      = {0 * LED_LIGHTNESS_PERCENT / 100  ,    0 * LED_LIGHTNESS_PERCENT / 100  ,      255 * LED_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_ORANGE_C]    = {0xc8 * LED_LIGHTNESS_PERCENT / 100,   0x32 * LED_LIGHTNESS_PERCENT / 100,     0x00 * LED_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_WHITE_C]     = {255 * LED_LIGHTNESS_PERCENT / 100,    255 * LED_LIGHTNESS_PERCENT / 100,      255 * LED_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_CYAN_C]      = {0 * LED_LIGHTNESS_PERCENT / 100  ,    255 * LED_LIGHTNESS_PERCENT / 100,      255 * LED_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_GOLD_C]      = {255 * LED_LIGHTNESS_PERCENT / 100,    215 * LED_LIGHTNESS_PERCENT / 100,      0 * LED_LIGHTNESS_PERCENT / 100},
    [SERIAL_LED_COLOR_SETTING_C]   = {0 * LED_LIGHTNESS_PERCENT / 100  ,    0 * LED_LIGHTNESS_PERCENT / 100  ,      0 * LED_LIGHTNESS_PERCENT / 100  },
    [SERIAL_LED_COLOR_NONE_C]      = {0 * LED_LIGHTNESS_PERCENT / 100  ,    0 * LED_LIGHTNESS_PERCENT / 100  ,      0 * LED_LIGHTNESS_PERCENT / 100  },
};

light_mode_para_t light_mode_para[] =
{
    [LIGHTS_MODE_ERROR] =
    {
        .color      = &led_color[SERIAL_LED_COLOR_RED_C],
        .period     = 50,
    },
    [LIGHTS_MODE_LOW_POWER] =
    {
        .color      = &led_color[SERIAL_LED_COLOR_RED_C],
        .period     = 50,
    },


};
color_t  front_left_color[3] =
{
    [0]     = {235, 130 , 5  },  //
    [1]     = {0  , 255, 0  },  //GREEN_C
    [2]     = {255, 165, 0  },  //ORANGE_C
};
color_t  front_right_color[3] =
{
    [0]     = {255, 0  , 0  },  //RED_C
    [1]     = {255, 165, 0  },  //ORANGE_C
    [2]     = {0  , 255, 0  },  //GREEN_C


};
color_t  back_left_color[3] =
{
    [0]     = {255, 255, 255},  //WHITE_C
    [1]     = {0  , 255, 0  },  //GREEN_C
    [2]     = {255, 165, 0  },  //ORANGE_C

};
color_t  back_right_color[3] =
{
    [0]     = {255, 0  , 0  },  //RED_C
    [1]     = {0  , 255, 0  },  //GREEN_C
    [2]     = {255, 165, 0  },  //ORANGE_C

};
//color_t  left_eye_color[2] =
//{
//    [0]     = {255, 255, 255},  //WHITE_C
//    [1]     = {0  , 0  , 0  },  //NONE_C
//};
//color_t  right_eye_color[2] =
//{
//    [0]     = {255, 255, 255},  //WHITE_C
//    [1]     = {0  , 0  , 0  },  //NONE_C
//};

one_wire_led_para_t one_wire_led[] =
{
    [FRONT_LEFT_LED] =
    {
        .gpio               = PLATFORM_GPIO_SERIAL_LED_FRONT_LEFT,
        .color              = front_left_color,
        .color_number       = 1,
        .period             = 5 * OS_TICKS_PER_SEC / 10,
        .data_buf           = front_left_buff,
        .led_num            = FRONT_LEFT_LED_NUM,
        .start_time         = 0,
    },
    [FRONT_RIGHT_LED] =
    {
        .gpio               = PLATFORM_GPIO_SERIAL_LED_FRONT_RIGHT,
        .color              = front_right_color,
        .color_number       = 1,
        .period             = 5 * OS_TICKS_PER_SEC / 10,
        .data_buf           = front_right_buff,
        .led_num            = FRONT_RIGHT_LED_NUM,
        .start_time         = 0,
    },
    [BACK_RIGHT_LED] =
    {
        .gpio               = PLATFORM_GPIO_SERIAL_LED_BACK_RIGHT,
        .color              = back_right_color,
        .color_number       = 1,
        .period             = 5 * OS_TICKS_PER_SEC / 10,
        .data_buf           = back_right_buff,
        .led_num            = BACK_RIGHT_LED_NUM,
        .start_time         = 0,
    },
    [BACK_LEFT_LED] =
    {
        .gpio               = PLATFORM_GPIO_SERIAL_LED_BACK_LEFT,
        .color              = back_left_color,
        .color_number       = 1,
        .period             = 5 * OS_TICKS_PER_SEC / 10,
        .data_buf           = back_left_buff,
        .led_num            = BACK_LEFT_LED_NUM,
        .start_time         = 0,
    },

//    [EYES_LED] =
//    {
//        .gpio               = PLATFORM_GPIO_SERIAL_LED_EYES,
//        .color              = right_eye_color,
//        .color_number       = 1,
//        .period             = 5 * OS_TICKS_PER_SEC / 10,
//        .data_buf           = eyes_buff,
//        .led_num            = EYES_LED_NUM,
//        .start_time         = 0,
//    },
};


static inline void write_bit_0(platform_gpio_e gpio)
{
    serial_led_output_high(gpio);
    delay_high_0();

    serial_led_output_low(gpio);
    delay_low_0();
}

static inline void write_bit_1(platform_gpio_e gpio)
{
    serial_led_output_high(gpio);
    delay_high_1();

    serial_led_output_low(gpio);
    delay_low_1();
}



void write_rgb(platform_gpio_e gpio, uint32_t word)
{
    uint8_t i;
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint32_t RGB;

    R = (word >> 8) & 0xff;
    G = (word >> 16) & 0xff;
    B = (word >> 0) & 0xff;

    RGB = (R << 16)|(G << 8)|(B << 0);

    for(i = 0; i < 24; i++)
    {

        if((RGB & 0x800000) == 0)
        {
            write_bit_0(gpio);
        }
        else
        {
            write_bit_1(gpio);
        }

        RGB <<= 1;

    }
}

static void send_rgb_data(one_wire_led_t led)
{
    uint8_t i = one_wire_led[led].led_num;

    while(i--)
    {
        write_rgb(one_wire_led[led].gpio, one_wire_led[led].data_buf[i] );
    }

}

#define LIGHTNESS   (1.25)
static void write_color(one_wire_led_t led, color_t *color)
{

    //uint32_t word = ((color->r/LIGHTNESS)<<16) | ((color->g/LIGHTNESS)<<8) | color->b/LIGHTNESS;
//    uint32_t word = (((color->r/6) * 1 )<<16) | (((color->g/6) * 1 )<<8) | ((color->b/6) * 1) ;
    uint32_t word = (color->r << 16) | (color->g << 8) | (color->b);
    uint8_t i = one_wire_led[led].led_num;


    while(i--)
    {
        one_wire_led[led].data_buf[i] = word;
    }
}

void open_eyes(void)
{
//    one_wire_led[EYES_LED].color[0] = led_color[SERIAL_LED_COLOR_WHITE_C];

//    one_wire_led[EYES_LED].color_number = 1;

}

void close_eyes(void)
{
//    one_wire_led[EYES_LED].color[0] = led_color[SERIAL_LED_COLOR_NONE_C];

//    one_wire_led[EYES_LED].color_number = 1;

}

#define SHINE_HIGH_SPEED_PERIOD         3 * OS_TICKS_PER_SEC / 10
#define SHINE_MEDIUM_SPEED_PERIOD       6 * OS_TICKS_PER_SEC / 10
#define SHINE_LOW_SPEED_PERIOD          1 * OS_TICKS_PER_SEC
#define UPDATE_PERIOD                   1 * OS_TICKS_PER_SEC
void set_serial_leds_effect(const light_mode_t light_mode, color_t  *cur_color, const uint8_t period)
{
    static  light_mode_t pre_mode = LIGHTS_MODE_NONE;
    static  color_t      pre_color;
    static  uint8_t      pre_period;
    if((light_mode == pre_mode) && (light_mode != LIGHTS_MODE_SETTING))
    {
        return;
    }

    if(light_mode == LIGHTS_MODE_SETTING)
    {
        if( (pre_color.b == cur_color->b) && (pre_color.g == cur_color->g) && (pre_color.r == cur_color->r))
        {
            if(pre_period == period)
            {
                return;
            }
        }
    }

    pre_mode = light_mode;
    memcpy(&pre_color, cur_color, sizeof(color_t));
    pre_period = period;

    switch(light_mode)
    {
        case LIGHTS_MODE_NORMAL:
            for(uint8_t i = FRONT_LEFT_LED; i <= BACK_RIGHT_LED; i++)
            {
                one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_ORANGE_C];
                one_wire_led[(one_wire_led_t)i].color[1] = led_color[SERIAL_LED_COLOR_NONE_C];
                one_wire_led[(one_wire_led_t)i].color_number = 2;
                one_wire_led[(one_wire_led_t)i].period = SHINE_MEDIUM_SPEED_PERIOD;
                one_wire_led[(one_wire_led_t)i].tick = 0;
            }
            open_eyes();
            break;
        case LIGHTS_MODE_ERROR:
            for(uint8_t i = FRONT_LEFT_LED; i <= BACK_RIGHT_LED; i++)
            {
                one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_RED_C];
                one_wire_led[(one_wire_led_t)i].color[1] = led_color[SERIAL_LED_COLOR_NONE_C];
                one_wire_led[(one_wire_led_t)i].color_number = 2;
                one_wire_led[(one_wire_led_t)i].period = SHINE_MEDIUM_SPEED_PERIOD;
                one_wire_led[(one_wire_led_t)i].tick = 0;
            }
            close_eyes();
            break;
        case LIGHTS_MODE_COM_ERROR:
            for(uint8_t i = FRONT_LEFT_LED; i <= BACK_RIGHT_LED; i++)
            {
                one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_RED_C];
                one_wire_led[(one_wire_led_t)i].color[1] = led_color[SERIAL_LED_COLOR_NONE_C];
                one_wire_led[(one_wire_led_t)i].color_number = 2;
                one_wire_led[(one_wire_led_t)i].period = SHINE_MEDIUM_SPEED_PERIOD;
                one_wire_led[(one_wire_led_t)i].tick = 0;
            }
            close_eyes();
            break;

        case LIGHTS_MODE_LOW_POWER:
            for(uint8_t i = FRONT_LEFT_LED; i <= BACK_RIGHT_LED; i++)
            {
                one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_RED_C];
                one_wire_led[(one_wire_led_t)i].color[1] = led_color[SERIAL_LED_COLOR_NONE_C];
                one_wire_led[(one_wire_led_t)i].color_number = 2;
                one_wire_led[(one_wire_led_t)i].period = SHINE_HIGH_SPEED_PERIOD;
                one_wire_led[(one_wire_led_t)i].tick = 0;
            }
            close_eyes();
            break;
        case LIGHTS_MODE_CHARGING_POWER_LOW:

            for(uint8_t i = FRONT_LEFT_LED; i <= BACK_RIGHT_LED; i++)
            {
                one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_RED_C];
                one_wire_led[(one_wire_led_t)i].color[1] = led_color[SERIAL_LED_COLOR_ORANGE_C];
                one_wire_led[(one_wire_led_t)i].color_number = 2;
                one_wire_led[(one_wire_led_t)i].period = SHINE_LOW_SPEED_PERIOD;
                one_wire_led[(one_wire_led_t)i].tick = 0;
            }

            close_eyes();
            break;
        case LIGHTS_MODE_CHARGING_POWER_MEDIUM:

            for(uint8_t i = FRONT_LEFT_LED; i <= BACK_RIGHT_LED; i++)
            {
                one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_ORANGE_C];
                one_wire_led[(one_wire_led_t)i].color[1] = led_color[SERIAL_LED_COLOR_GREEN_C];
                one_wire_led[(one_wire_led_t)i].color_number = 2;
                one_wire_led[(one_wire_led_t)i].period = SHINE_LOW_SPEED_PERIOD;
                one_wire_led[(one_wire_led_t)i].tick = 0;
            }

            close_eyes();
            break;
        case LIGHTS_MODE_CHARGING_FULL:

            for(uint8_t i = FRONT_LEFT_LED; i <= BACK_RIGHT_LED; i++)
            {
                one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_GREEN_C];
                one_wire_led[(one_wire_led_t)i].color_number = 1;
                one_wire_led[(one_wire_led_t)i].period = UPDATE_PERIOD;
                one_wire_led[(one_wire_led_t)i].tick = 0;
            }

            close_eyes();
            break;
        case LIGHTS_MODE_TURN_LEFT:
            for(uint8_t i = FRONT_LEFT_LED; i <= BACK_RIGHT_LED; i++)
            {
                if((i == FRONT_LEFT_LED) || ( i == BACK_LEFT_LED))
                {
                    one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_ORANGE_C];
                }
                else
                {
                    one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_NONE_C];
                }

                one_wire_led[(one_wire_led_t)i].color[1] = led_color[SERIAL_LED_COLOR_NONE_C];
                one_wire_led[(one_wire_led_t)i].color_number = 2;
                one_wire_led[(one_wire_led_t)i].period = SHINE_MEDIUM_SPEED_PERIOD;
                one_wire_led[(one_wire_led_t)i].tick = 0;
            }
            open_eyes();
            break;
        case LIGHTS_MODE_TURN_RIGHT:
            for(uint8_t i = FRONT_LEFT_LED; i <= BACK_RIGHT_LED; i++)
            {
                if((i == FRONT_RIGHT_LED) || ( i == BACK_RIGHT_LED))
                {
                    one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_ORANGE_C];
                }
                else
                {
                    one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_NONE_C];
                }

                one_wire_led[(one_wire_led_t)i].color[1] = led_color[SERIAL_LED_COLOR_NONE_C];
                one_wire_led[(one_wire_led_t)i].color_number = 2;
                one_wire_led[(one_wire_led_t)i].period = SHINE_MEDIUM_SPEED_PERIOD;
                one_wire_led[(one_wire_led_t)i].tick = 0;
            }
            open_eyes();
            break;
        case LIGHTS_MODE_EMERGENCY_STOP:
            for(uint8_t i = FRONT_LEFT_LED; i <= BACK_RIGHT_LED; i++)
            {
                one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_RED_C];//led_color[WHITE_C];
                one_wire_led[(one_wire_led_t)i].color_number = 1;
                one_wire_led[(one_wire_led_t)i].period = UPDATE_PERIOD;
                one_wire_led[(one_wire_led_t)i].tick = 0;
            }
            close_eyes();
            break;
        case LIGHTS_MODE_SETTING:
            for(uint8_t i = FRONT_LEFT_LED; i <= BACK_RIGHT_LED; i++)
            {
                memcpy(&led_color[SERIAL_LED_COLOR_SETTING_C], cur_color, sizeof(color_t));
                one_wire_led[(one_wire_led_t)i].color[0] = led_color[SERIAL_LED_COLOR_SETTING_C];
                one_wire_led[(one_wire_led_t)i].color[1] = led_color[SERIAL_LED_COLOR_NONE_C];
                one_wire_led[(one_wire_led_t)i].color_number = 2;
                one_wire_led[(one_wire_led_t)i].period = period * 10;
                one_wire_led[(one_wire_led_t)i].tick = 0;
            }
            open_eyes();
            break;
        default :
            break;
    }
}

void serial_leds_tick(void)
{
    for(uint8_t i = FRONT_LEFT_LED; i < LED_NONE; i++)
    {
        if(get_tick() - one_wire_led[i].start_time >= one_wire_led[i].period)
        {
            one_wire_led[i].tick++;
            one_wire_led[i].start_time = get_tick();

            if(one_wire_led[i].color_number <= 2)
            {
                write_color((one_wire_led_t)i, &(one_wire_led[i].color[one_wire_led[i].tick % one_wire_led[i].color_number]));
                OS_ENTER_CRITICAL();
                send_rgb_data((one_wire_led_t)i);
                OS_EXIT_CRITICAL();
            }
        }
    }
}

