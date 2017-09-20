


#include "fan.h"

fan_ctrl_t fan_1 = 
{
    .percent = 80,
    .frq = 50000,
};

fan_ctrl_t fan_2 = 
{
    .percent = 80,
    .frq = 50000,
};

fan_ctrl_t fan_3 = 
{
    .percent = 20,
    .frq = 50000,
};

void FanInit(void)
{
    platform_pin_config_t pin_config;
    pin_config.gpio_speed = GPIO_SPEED_HIGH;
    pin_config.gpio_mode = GPIO_MODE_OUTPUT_PP;
    pin_config.gpio_pull = GPIO_PULLUP;
    MicoGpioInitialize( (mico_gpio_t)MICO_GPIO_FAN_1_CTRL, &pin_config );
    MicoGpioInitialize( (mico_gpio_t)MICO_GPIO_FAN_2_CTRL, &pin_config );
    
    //pin_config.gpio_mode = GPIO_MODE_AF_PP;
    
    MicoGpioInitialize( (mico_gpio_t)MICO_GPIO_FAN_3_CTRL, &pin_config );

    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_FAN_1_CTRL );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_FAN_2_CTRL );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_FAN_3_CTRL );

    MicoPwmInitialize( MICO_PWM_1, fan_1.frq, fan_1.percent );
    MicoPwmInitialize( MICO_PWM_2, fan_2.frq, fan_2.percent );
    MicoPwmInitialize( MICO_PWM_3, fan_3.frq, fan_3.percent );
    
    MicoPwmStart( MICO_PWM_1 );
    MicoPwmStart( MICO_PWM_2 );
    MicoPwmStartEx( MICO_PWM_3 );
  
}