/*
 *  Author: Kaka Xie
 *  brief: platform
 */

#include "platform.h"
#include "stdio.h"
#include "battery.h"
#include "led.h"
#include "can.h"
#include "timer.h"
#include "sys.h"



const platform_gpio_t platform_gpio_pins[] =
{
    ////////////////////////////////////////////
    [PLATFORM_GPIO_CAN_RX]            = { GPIOA, GPIO_Pin_11 },
    [PLATFORM_GPIO_CAN_TX]            = { GPIOA, GPIO_Pin_12 },
    [PLATFORM_GPIO_CAN_STB]           = { GPIOD, GPIO_Pin_3 },

    [PLATFORM_GPIO_UART4_TX]          = { GPIOC, GPIO_Pin_10 },
    [PLATFORM_GPIO_UART4_RX]          = { GPIOC, GPIO_Pin_11 },

    [PLATFORM_GPIO_UART3_TX]          = { GPIOB, GPIO_Pin_10 },
    [PLATFORM_GPIO_UART3_RX]          = { GPIOB, GPIO_Pin_11 },

    [PLATFORM_GPIO_UART2_TX]          = { GPIOD,  GPIO_Pin_5 },
    [PLATFORM_GPIO_UART2_RX]          = { GPIOD,  GPIO_Pin_6 },

    [PLATFORM_GPIO_UART1_TX]          = { GPIOA,  GPIO_Pin_9 },
    [PLATFORM_GPIO_UART1_RX]          = { GPIOA, GPIO_Pin_10 },

    [PLATFORM_GPIO_SYS_LED]             = { GPIOC,  GPIO_Pin_9 },
    [PLATFORM_GPIO_CHARGE_IN]           = { GPIOB,  GPIO_Pin_7 },
    [PLATFORM_GPIO_RECHARGE_IN]         = { GPIOB,  GPIO_Pin_6 },
    [PLATFORM_GPIO_RECHARGE_LED]        = { GPIOC,  GPIO_Pin_8 },
    [PLATFORM_GPIO_PWRKEY]              = { GPIOD,  GPIO_Pin_4 },
    [PLATFORM_GPIO_SWITCH_EN]           = { GPIOD,  GPIO_Pin_7 },
    [PLATFORM_GPIO_SWITCH_SEL0]         = { GPIOG,  GPIO_Pin_9 },
    [PLATFORM_GPIO_SWITCH_SEL1]         = { GPIOG, GPIO_Pin_10 },
    [PLATFORM_GPIO_SWITCH_SEL2]         = { GPIOG, GPIO_Pin_11 },
    [PLATFORM_GPIO_SWITCH_SEL3]         = { GPIOG, GPIO_Pin_12 },




    [PLATFORM_GPIO_5V_MOTOR_EN]         = { GPIOE, GPIO_Pin_2 },
    [PLATFORM_GPIO_5V_RECHARGE_EN]      = { GPIOE, GPIO_Pin_3 },
    [PLATFORM_GPIO_5V_SENSOR_BOARD_EN]  = { GPIOE, GPIO_Pin_5 },
    [PLATFORM_GPIO_5V_SWITCH_EN]        = { GPIOE, GPIO_Pin_14},
    [PLATFORM_GPIO_12V_2_1_PA_EN]       = { GPIOF, GPIO_Pin_2 },
    [PLATFORM_GPIO_12V_EXTEND_EN]       = { GPIOF, GPIO_Pin_3 },
    [PLATFORM_GPIO_12V_X86_EN]          = { GPIOF, GPIO_Pin_4 },
    [PLATFORM_GPIO_12V_NV_EN]           = { GPIOF, GPIO_Pin_5 },
    [PLATFORM_GPIO_5V_EN]               = { GPIOE,  GPIO_Pin_7 },
    [PLATFORM_GPIO_12V_EN]              = { GPIOE,  GPIO_Pin_8 },
    [PLATFORM_GPIO_24V_EN]              = { GPIOE,  GPIO_Pin_9 },
    [PLATFORM_GPIO_485_EN]              = { GPIOD,  GPIO_Pin_0 },

    [PLATFORM_GPIO_12V_PAD_EN]          = { GPIOF, GPIO_Pin_0 },
    [PLATFORM_GPIO_PWR_NV]              = { GPIOF, GPIO_Pin_13 },
    [PLATFORM_GPIO_PWR_PAD]             = { GPIOF, GPIO_Pin_15 },
    [PLATFORM_GPIO_PWR_X86]             = { GPIOG,  GPIO_Pin_0 },
    [PLATFORM_GPIO_PWR_RESERVE]         = { GPIOG,  GPIO_Pin_1 },

    [PLATFORM_GPIO_5V_ROUTER_EN]        = { GPIOG,  GPIO_Pin_7 },
    [PLATFORM_GPIO_VSYS_24V_NV_EN]      = { GPIOG,  GPIO_Pin_6 },
    [PLATFORM_GPIO_SLAM_EN]             = { GPIOG, GPIO_Pin_5 },
    [PLATFORM_GPIO_24V_PRINTER_EN]      = { GPIOG, GPIO_Pin_4 },
    [PLATFORM_GPIO_24V_EXTEND_EN]       = { GPIOG, GPIO_Pin_2 },

    [PLATFORM_GPIO_IRLED_PWM]           = { GPIOA,  GPIO_Pin_8 },

    [PLATFORM_GPIO_CHARGE_ADC]          = { GPIOF, GPIO_Pin_6 },
    [PLATFORM_GPIO_BATIN_ADC]           = { GPIOF, GPIO_Pin_7 },
    [PLATFORM_GPIO_VBUS_ADC]            = { GPIOF, GPIO_Pin_8 },
    [PLATFORM_GPIO_BAT_MOTOR_ADC]       = { GPIOF, GPIO_Pin_9 },
    [PLATFORM_GPIO_MULTI_CHANNEL_ADC]   = { GPIOF, GPIO_Pin_10},
    [PLATFORM_GPIO_12V_2_1_PA_ADC]      = { GPIOC, GPIO_Pin_0 },
    [PLATFORM_GPIO_12V_PAD_ADC]         = { GPIOC, GPIO_Pin_1 },
    [PLATFORM_GPIO_24V_PRINTER_ADC]     = { GPIOC, GPIO_Pin_2 },
    [PLATFORM_GPIO_12V_X86_ADC]         = { GPIOC, GPIO_Pin_3 },
    [PLATFORM_GPIO_5V_RES1_ADC]         = { GPIOA, GPIO_Pin_0 },
    [PLATFORM_GPIO_VSYS_24V_NV_ADC]     = { GPIOA, GPIO_Pin_2 },
    [PLATFORM_GPIO_12V_NV_ADC]          = { GPIOA, GPIO_Pin_3 },
//    [PLATFORM_GPIO_48V_EXTEND_ADC]      = { GPIOA, GPIO_Pin_4 },
    [PLATFORM_GPIO_12V_EXTEND_ADC]      = { GPIOA, GPIO_Pin_5 },
    [PLATFORM_GPIO_RECHARGE_ADC]        = { GPIOA, GPIO_Pin_6 },
    [PLATFORM_GPIO_24V_EXTEND_ADC]      = { GPIOA, GPIO_Pin_7 },
    [PLATFORM_GPIO_IRLED_ADC]           = { GPIOC, GPIO_Pin_4 },
    [PLATFORM_GPIO_5V_LEDS_ADC]         = { GPIOC, GPIO_Pin_5 },
//    [PLATFORM_GPIO_5V_MOTOR_ADC]        = { GPIOB, GPIO_Pin_0 },
    [PLATFORM_GPIO_24V_SLAM_ADC]        = { GPIOB, GPIO_Pin_1 },

    [PLATFORM_GPIO_5V_POLE_MOTOR_ADC]   = { GPIOA, GPIO_Pin_1 },

    [PLATFORM_GPIO_LED_MCU_POWER_EN]    = { GPIOE, GPIO_Pin_10},
    [PLATFORM_GPIO_LED_MCU_RESET]       = { GPIOE, GPIO_Pin_11},

    [PLATFORM_GPIO_CHARGE_FAN_CTRL]     = { GPIOD, GPIO_Pin_1},
    [PLATFORM_GPIO_FAN_1_CTRL]          = { GPIOB, GPIO_Pin_9},
    [PLATFORM_GPIO_FAN_2_CTRL]          = { GPIOB, GPIO_Pin_8},
    [PLATFORM_GPIO_FAN_3_CTRL]          = { GPIOB, GPIO_Pin_15},


    [PLATFORM_GPIO_5V_KEYPAD_EN]        = { GPIOE, GPIO_Pin_15},
    [PLATFORM_GPIO_CAMERA_FRONT_LED_EN] = { GPIOG, GPIO_Pin_13},
    [PLATFORM_GPIO_CAMERA_BACK_LED_EN]  = { GPIOG, GPIO_Pin_14},
    [PLATFORM_GPIO_PWR_CTRL_OUT]        = { GPIOD, GPIO_Pin_10},
    [PLATFORM_GPIO_PWR_CTRL_IN]         = { GPIOD, GPIO_Pin_10},
    [PLATFORM_GPIO_3_3V_DOOR_CTRL]      = { GPIOG, GPIO_Pin_15},

    [PLATFORM_GPIO_HW_VERSION_ID_0]     = { GPIOE, GPIO_Pin_0 },
    [PLATFORM_GPIO_HW_VERSION_ID_1]     = { GPIOE, GPIO_Pin_1 },

    [PLATFORM_GPIO_3V3_CARD_EN_1]       = { GPIOC, GPIO_Pin_7 },
    [PLATFORM_GPIO_3V3_CARD_EN_2]       = { GPIOC, GPIO_Pin_6 },
    [PLATFORM_GPIO_3V3_CARD_EN_3]       = { GPIOE, GPIO_Pin_6 },
    [PLATFORM_GPIO_3V3_CARD_EN_4]       = { GPIOE, GPIO_Pin_12},

    [PLATFORM_GPIO_FAN_12V_DC_CTRL]     = { GPIOA, GPIO_Pin_15},
    [PLATFORM_GPIO_BEEPER_CTRL]         = { GPIOB, GPIO_Pin_0 },

};



uint32_t get_tick(void)
{
    return OSTimeGet();
}

static void input_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    /*GPIO_B*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    /*GPIO_D*/
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_Init(GPIOD, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOD, &GPIO_InitStructure);


    /*GPIO_E*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

}

static void output_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /*GPIO_A*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*GPIO_B*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*GPIO_C*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*GPIO_D*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /*GPIO_E*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /*GPIO_F*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    /*GPIO_G*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

static void charge_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    EXTI_InitTypeDef exit_init_structure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //ʹ��ʱ��


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7);

    exit_init_structure.EXTI_Line = EXTI_Line6 | EXTI_Line7;
    exit_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
    exit_init_structure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    exit_init_structure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exit_init_structure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


static void init_reset_gpio(void)
{
    GPIO_ResetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1);
    GPIO_ResetBits(GPIOF, GPIO_Pin_15);
}

static void init_set_gpio(void)
{
    GPIO_SetBits(GPIOD, GPIO_Pin_10 | GPIO_Pin_14);
    GPIO_SetBits(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
    GPIO_SetBits(GPIOG, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8);
}

static void platform_gpio_init(void)
{
    output_gpio_init();
    input_gpio_init();
    init_set_gpio();
    init_reset_gpio();
    charge_gpio_init();
}

static void switch_init(void)
{

}

uint8_t get_switch_state(void)
{
    return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11);
}

void hold_on_power(void)
{
    GPIO_SetBits(GPIOD, GPIO_Pin_10);
}

void release_power(void)
{
    GPIO_ResetBits(GPIOD, GPIO_Pin_10);
}

void main_power_module_5v_ctrl(uint8_t on_off)
{
    if(on_off == MODULE_POWER_ON)
    {
        GPIO_ResetBits(GPIOE, GPIO_Pin_7);
    }
    else if(on_off == MODULE_POWER_OFF)
    {
        GPIO_SetBits(GPIOE, GPIO_Pin_7);
    }
}

void x86_power_signal_ctrl(uint8_t on_off)
{
    if(on_off == MODULE_POWER_ON)
    {
        GPIO_SetBits(GPIOG, GPIO_Pin_0);
    }
    else if(on_off == MODULE_POWER_OFF)
    {
        GPIO_ResetBits(GPIOG, GPIO_Pin_0);
    }
}

void rk_power_signal_ctrl(uint8_t on_off)
{
    if(on_off == MODULE_POWER_ON)
    {
        GPIO_SetBits(GPIOF, GPIO_Pin_15);
    }
    else if(on_off == MODULE_POWER_OFF)
    {
        GPIO_ResetBits(GPIOF, GPIO_Pin_15);
    }
}

void beeper_on(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

void beeper_off(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

void main_power_module_12v_ctrl(uint8_t on_off)
{
    if(on_off == MODULE_POWER_ON)
    {
        GPIO_ResetBits(GPIOE, GPIO_Pin_8);
    }
    else if(on_off == MODULE_POWER_OFF)
    {
        GPIO_SetBits(GPIOE, GPIO_Pin_8);
    }
}

void main_power_module_24v_ctrl(uint8_t on_off)
{
    if(on_off == MODULE_POWER_ON)
    {
        GPIO_ResetBits(GPIOE, GPIO_Pin_9);
    }
    else if(on_off == MODULE_POWER_OFF)
    {
        GPIO_SetBits(GPIOE, GPIO_Pin_9);
    }
}

void device_ctrl(uint32_t power_en, uint8_t on_off)
{

}

void ir_led_pwm_ctrl(uint16_t duty)
{
    if(duty <= 100)
    {
        timer_1_ch1_pwm_init(720, 1, 720 * duty / 100 );
    }

}

static void ir_led_pwm_init(void)
{
    timer_1_ch1_pwm_init(720 , 1, 720 * 20 / 100);
}

void hardware_init(void)
{
    platform_gpio_init();
    bat_uart_init();
    led_init();
    can_init();
//    ir_led_pwm_ctrl(13);
    ir_led_pwm_init();
}



