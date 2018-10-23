/*
 *  Author: Adam Huang
 *  Date:2016/6/15
 */
#ifndef __SERIAL_LED_H__
#define __SERIAL_LED_H__
#include <stdint.h>
#include "Common.h"

//#include "platform.h"
#include "mico_platform.h"


#define BITBAND(addr, bitnum)       ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)              *((__IO uint32_t*)(addr))

#define GPIOout(GPIOx,bit)          MEM_ADDR(BITBAND((uint32_t)(&GPIOx->ODR), bit))
#define GPIOin(GPIOx,bit)           MEM_ADDR(BITBAND((uint32_t)(&GPIOx->IDR), bit))


typedef enum
{
    LIGHTS_MODE_NONE                    = 0,
    LIGHTS_MODE_NOMAL                   = 1,
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



#if 1
#define asm            __asm
#define delay_300ns()     do {asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");} while(1==0)

#define delay_600ns()     do { asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");} while(1==0)

#define delay_us(n)       do { for(uint32_t i=0;i<n;i++){delay_300ns();delay_600ns();}\
} while(0==1)


#define delay_200ns()     do {asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
} while(1==0)

#define delay_500ns()     do { asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");asm("nop");asm("nop");\
    asm("nop");asm("nop");} while(1==0)
//#else

#define delay_ms          HAL_Delay
#endif

#define FRONT_LEFT_LED_NUM          16
#define FRONT_RIGHT_LED_NUM         16
#define BACK_RIGHT_LED_NUM          16
#define BACK_LEFT_LED_NUM           16
#define LEFT_EYE_LED_NUM            16
#define RIGHT_EYE_LED_NUM           16

#if 1
#define  LedOutputHigh(gpio)    platform_gpio_pins[gpio].port->BSRR = (uint16_t) ( 1 << platform_gpio_pins[gpio].pin_number )
#define  LedOutputLow(gpio)     platform_gpio_pins[gpio].port->BSRR = (uint32_t) ( 1 << platform_gpio_pins[gpio].pin_number ) << 16;
#else
#define  LedOutputHigh(gpio)    gpio.port->BSRR = (uint16_t) ( 1 << gpio.pin_number )
#define  LedOutputLow(gpio)     gpio.port->BSRR = (uint32_t) ( 1 << gpio.pin_number ) << 16;
#endif
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

typedef enum
{
    RED_C = 0,
    GREEN_C,
    BLUE_C,
    ORANGE_C,
    WHITE_C,
    CYAN_C,
    GOLD_C,

    SETTING_C,
    NONE_C,

}led_color_t;

typedef enum
{
    FRONT_LEFT_LED = 0,
    FRONT_RIGHT_LED,
    BACK_LEFT_LED,
    BACK_RIGHT_LED,

    LEFT_EYE_LED,
    RIGHT_EYE_LED,

    LED_NONE,
}one_wire_led_t;

typedef struct
{
    mico_gpio_t     gpio;
    color_t         *color;
    uint8_t         color_number;
    uint16_t        period;
    __IO uint32_t   *data_buf;
    uint8_t         led_num;
    uint32_t        start_time;
    uint32_t        tick;
}one_wire_led_para_t;


void write_0(void);
void write_1(void);

void write_24bit(uint32_t word);
void reset_led(void);

void set_serial_leds_effect( light_mode_t lights_mode, color_t *color, uint8_t period );
void get_serials_leds_version(void);

void leds_protocol_period(void);
#endif

