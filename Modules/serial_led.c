/*
 *  Author: Kaka Xie
 *  brief: serial leds control
 */

#include "serial_led.h"
#include "delay.h"


__IO uint32_t front_left_buff[FRONT_LEFT_LED_NUM] = {0};
__IO uint32_t front_right_buff[FRONT_RIGHT_LED_NUM] = {0};
__IO uint32_t back_right_buff[BACK_RIGHT_LED_NUM] = {0};
__IO uint32_t back_left_buff[BACK_LEFT_LED_NUM] = {0};
__IO uint32_t eyes_buff[EYES_LED_NUM] = {0};

color_t charge_color[] =
{
    [0]     = {255, 0  , 0  },  //RED_C
    [1]     = {255, 165, 0  },  //ORANGE_C
    [2]     = {0  , 255, 0  },  //GREEN_C
    [3]     = {255, 255, 255},  //WHITE_C
    [4]     = {0  , 255, 255},  //CYAN_C

};
color_t led_color[] =
{
    [SERIAL_LED_COLOR_RED_C]       = {255, 0  , 0  },
    [SERIAL_LED_COLOR_GREEN_C]     = {0  , 255, 0  },
    [SERIAL_LED_COLOR_BLUE_C]      = {0  , 0  , 255},
    [SERIAL_LED_COLOR_ORANGE_C]    = {0xc8, 0x32, 0x00 },
    [SERIAL_LED_COLOR_WHITE_C]     = {255, 255, 255},
    [SERIAL_LED_COLOR_CYAN_C]      = {0  , 255, 255},
    [SERIAL_LED_COLOR_GOLD_C]      = {255, 215, 0  },
    [SERIAL_LED_COLOR_SETTING_C]   = {0  , 0  , 0  },
    [SERIAL_LED_COLOR_NONE_C]      = {0  , 0  , 0  },
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
color_t  left_eye_color[2] =
{
    [0]     = {255, 255, 255},  //WHITE_C
    [1]     = {0  , 0  , 0  },  //NONE_C
};
color_t  right_eye_color[2] =
{
    [0]     = {255, 255, 255},  //WHITE_C
    [1]     = {0  , 0  , 0  },  //NONE_C
};

one_wire_led_para_t one_wire_led[] =
{
    [FRONT_RIGHT_LED] =
    {
        .gpio               = PLATFORM_GPIO_SERIAL_LED_FRONT_RIGHT,
        .color              = front_right_color,
        .color_number       = 1,
        .period             = 500,
        .data_buf           = front_right_buff,
        .led_num            = FRONT_RIGHT_LED_NUM,
        .start_time         = 0,
    },
    [FRONT_LEFT_LED] =
    {
        .gpio               = PLATFORM_GPIO_SERIAL_LED_FRONT_LEFT,
        .color              = front_left_color,
        .color_number       = 1,
        .period             = 500,
        .data_buf           = front_left_buff,
        .led_num            = FRONT_LEFT_LED_NUM,
        .start_time         = 0,
    },
    [BACK_RIGHT_LED] =
    {
        .gpio               = PLATFORM_GPIO_SERIAL_LED_BACK_RIGHT,
        .color              = back_right_color,
        .color_number       = 1,
        .period             = 500,
        .data_buf           = back_right_buff,
        .led_num            = BACK_RIGHT_LED_NUM,
        .start_time         = 0,
    },
    [BACK_LEFT_LED] =
    {
        .gpio               = PLATFORM_GPIO_SERIAL_LED_BACK_LEFT,
        .color              = back_left_color,
        .color_number       = 1,
        .period             = 500,
        .data_buf           = back_left_buff,
        .led_num            = BACK_LEFT_LED_NUM,
        .start_time         = 0,
    },

    [EYES_LED] =
    {
        .gpio               = PLATFORM_GPIO_SERIAL_LED_EYES,
        .color              = right_eye_color,
        .color_number       = 1,
        .period             = 500,
        .data_buf           = eyes_buff,
        .led_num            = EYES_LED_NUM,
        .start_time         = 0,
    },
};


void write_bit_0(platform_gpio_e gpio)
{
    serial_led_output_high(gpio);
    delay_200ns();

    serial_led_output_low(gpio);
    delay_200ns();
    delay_500ns();
}

void write_bit_1(platform_gpio_e gpio)
{
    serial_led_output_high(gpio);
    delay_500ns();

    serial_led_output_low(gpio);
    delay_500ns();
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

    for(i=0;i<24;i++)
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
    uint32_t word = (((color->r/5) * 4 )<<16) | (((color->g/5) * 4 )<<8) | ((color->b/5) * 4) ;
    uint8_t i = one_wire_led[led].led_num;


    while(i--)
    {
        one_wire_led[led].data_buf[i] = word;
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
        }

        if(one_wire_led[i].color_number <= sizeof(charge_color)/sizeof(charge_color[0]))
        {

            write_color((one_wire_led_t)i, &(one_wire_led[i].color[one_wire_led[i].tick % one_wire_led[i].color_number]));
#if 1
//            DISABLE_INTERRUPTS();
            send_rgb_data((one_wire_led_t)i);
//            ENABLE_INTERRUPTS();
#endif
        }
    }


}

