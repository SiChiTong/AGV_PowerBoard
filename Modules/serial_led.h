#ifndef __MODULE_SERIAL_LED_H__
#define __MODULE_SERIAL_LED_H__
#include "sys.h"
#include "platform.h"

#define serial_led_output_high(gpio)    platform_gpio_pins[gpio].GPIOx->BSRR = platform_gpio_pins[gpio].GPIO_Pin
#define serial_led_output_low(gpio)     platform_gpio_pins[gpio].GPIOx->BRR = platform_gpio_pins[gpio].GPIO_Pin

#define FRONT_LEFT_LED_NUM          45
#define FRONT_RIGHT_LED_NUM         20
#define BACK_RIGHT_LED_NUM          45
#define BACK_LEFT_LED_NUM           20

#define EYES_LED_NUM                5

typedef enum
{
    LIGHTS_MODE_NONE                    = 0,
    LIGHTS_MODE_NORMAL                   = 1,
    LIGHTS_MODE_ERROR                   = 2,
    LIGHTS_MODE_LOW_POWER,
    LIGHTS_MODE_CHARGING_POWER_MEDIUM,
    LIGHTS_MODE_CHARGING_POWER_LOW,
    LIGHTS_MODE_CHARGING_FULL,
    LIGHTS_MODE_TURN_LEFT,
    LIGHTS_MODE_TURN_RIGHT,
    LIGHTS_MODE_COM_ERROR,
    LIGHTS_MODE_EMERGENCY_STOP,


    LIGHTS_MODE_SETTING                 = 0xff,
}light_mode_t;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}color_t;

typedef struct
{
    color_t     *color;
    uint8_t     period;
}light_mode_para_t;

typedef struct
{
    uint16_t shine_period;
    uint8_t led_num;
    uint8_t on_off;
    color_t color;
}serial_led_ctrl_t;


typedef enum
{
    SERIAL_LED_COLOR_RED_C = 0,
    SERIAL_LED_COLOR_GREEN_C,
    SERIAL_LED_COLOR_BLUE_C,
    SERIAL_LED_COLOR_ORANGE_C,
    SERIAL_LED_COLOR_WHITE_C,
    SERIAL_LED_COLOR_CYAN_C,
    SERIAL_LED_COLOR_GOLD_C,

    SERIAL_LED_COLOR_SETTING_C,
    SERIAL_LED_COLOR_NONE_C,

}led_color_t;

typedef enum
{
    FRONT_LEFT_LED = 0,
    FRONT_RIGHT_LED,
    BACK_LEFT_LED,
    BACK_RIGHT_LED,
//    EYES_LED,

    LED_NONE,

}one_wire_led_t;

typedef struct
{
    const platform_gpio_e   gpio;
    color_t                 color[5];
    uint8_t                 color_number;
    uint16_t                period;
    volatile uint32_t       *data_buf;
    uint8_t                 led_num;
    volatile uint32_t       start_time;
    volatile uint32_t       tick;
}one_wire_led_para_t;

void serial_leds_tick(void);
void set_serial_leds_effect(const light_mode_t light_mode, color_t  *cur_color, const uint8_t period);

#endif
