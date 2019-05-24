/*
 *  Author: Kaka Xie
 *  brief: platform
 */

#include "platform.h"
#include "delay.h"
#include "stdio.h"
#include "battery.h"
#include "led.h"
#include "can.h"
#include "timer.h"
#include "sys.h"
#include "id.h"

sys_status_t sys_status_ram = {0};
sys_status_t *sys_status = &sys_status_ram;

const platform_gpio_t platform_gpio_pins[] =
{
#if 0
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
#endif

    [PLATFORM_GPIO_SYS_LED]             = { GPIOC,  GPIO_Pin_9 },

    [PLATFORM_GPIO_PWRKEY]              = { GPIOD,  GPIO_Pin_4 },



//    [PLATFORM_GPIO_12V_EXTEND_EN]       = { GPIOF, GPIO_Pin_3 },
//    [PLATFORM_GPIO_12V_X86_EN]          = { GPIOF, GPIO_Pin_4 },
//    [PLATFORM_GPIO_12V_NV_EN]           = { GPIOF, GPIO_Pin_5 },
    [PLATFORM_GPIO_5V_EN]               = { GPIOE,  GPIO_Pin_7 },
    [PLATFORM_GPIO_12V_EN]              = { GPIOE,  GPIO_Pin_8 },
    [PLATFORM_GPIO_24V_EN]              = { GPIOE,  GPIO_Pin_9 },


//    [PLATFORM_GPIO_12V_PAD_EN]          = { GPIOF, GPIO_Pin_0 },
    [PLATFORM_GPIO_PWR_NV]              = { GPIOF, GPIO_Pin_13 },
    [PLATFORM_GPIO_PWR_PAD]             = { GPIOF, GPIO_Pin_15 },
    [PLATFORM_GPIO_PWR_X86]             = { GPIOG,  GPIO_Pin_0 },
    [PLATFORM_GPIO_PWR_RESERVE]         = { GPIOG,  GPIO_Pin_1 },


    [PLATFORM_GPIO_VSYS_24V_NV_EN]      = { GPIOG,  GPIO_Pin_6 },


    [PLATFORM_GPIO_IRLED_PWM]           = { GPIOA,  GPIO_Pin_8 },

    [PLATFORM_GPIO_LED_MCU_POWER_EN]    = { GPIOE, GPIO_Pin_10},
    [PLATFORM_GPIO_LED_MCU_RESET]       = { GPIOE, GPIO_Pin_11},

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

    [PLATFORM_GPIO_SERIAL_LED_FRONT_LEFT] = {GPIOA, GPIO_Pin_1},
    [PLATFORM_GPIO_SERIAL_LED_FRONT_RIGHT] = {GPIOA, GPIO_Pin_0},

    [PLATFORM_GPIO_SERIAL_LED_BACK_RIGHT] = {GPIOA, GPIO_Pin_7},
    [PLATFORM_GPIO_SERIAL_LED_BACK_LEFT] = {GPIOA, GPIO_Pin_6},
    [PLATFORM_GPIO_SERIAL_LED_EYES]     = {GPIOA, GPIO_Pin_5},

    [PLATFORM_GPIO_EVENT_BUTTON]        = {GPIOD, GPIO_Pin_6},

    [PLATFORM_GPIO_LED_CTRL_1]          = {GPIOG, GPIO_Pin_8},
    [PLATFORM_GPIO_LED_CTRL_2]          = {GPIOG, GPIO_Pin_7},
    [PLATFORM_GPIO_LED_CTRL_3]          = {GPIOG, GPIO_Pin_5},
    [PLATFORM_GPIO_LED_CTRL_4]          = {GPIOG, GPIO_Pin_4},
    [PLATFORM_GPIO_LED_CTRL_5]          = {GPIOD, GPIO_Pin_15},
    [PLATFORM_GPIO_LED_CTRL_6]          = {GPIOD, GPIO_Pin_14},

    [PLATFORM_GPIO_S_1]                 = {GPIOF, GPIO_Pin_0},
    [PLATFORM_GPIO_S_2]                 = {GPIOF, GPIO_Pin_1},
    [PLATFORM_GPIO_S_3]                 = {GPIOF, GPIO_Pin_2},
    [PLATFORM_GPIO_S_4]                 = {GPIOF, GPIO_Pin_3},
    [PLATFORM_GPIO_S_5]                 = {GPIOF, GPIO_Pin_4},
    [PLATFORM_GPIO_S_6]                 = {GPIOF, GPIO_Pin_5},
    [PLATFORM_GPIO_S_7]                 = {GPIOF, GPIO_Pin_6},
    [PLATFORM_GPIO_S_8]                 = {GPIOF, GPIO_Pin_7},
    [PLATFORM_GPIO_S_9]                 = {GPIOF, GPIO_Pin_8},

};



uint32_t get_tick(void)
{
    return OSTimeGet();
}


void mcu_restart(void)
{
    __set_FAULTMASK(1);//close all interrupt
    NVIC_SystemReset();//reset all
}


static void input_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /*GPIO_E*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

}

static void output_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /*GPIO_A*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_15;
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
    GPIO_SetBits(GPIOD, GPIO_Pin_10);//must hold on power
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_SetBits(GPIOD, GPIO_Pin_10);//must hold on power immediately

    /*GPIO_E*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | /*GPIO_Pin_10 | GPIO_Pin_11 |*/ GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
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
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    /*GPIO_D*/
    GPIO_SetBits(GPIOD, GPIO_Pin_10);//must hold on power
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, GPIO_Pin_8);
}

static void charge_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    EXTI_InitTypeDef exit_init_structure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   // πƒ‹ ±÷”


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
    GPIO_ResetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3);
    GPIO_ResetBits(GPIOF, GPIO_Pin_15);
}

static void init_set_gpio(void)
{
    GPIO_SetBits(GPIOD, GPIO_Pin_10 | GPIO_Pin_13 | GPIO_Pin_14);
    GPIO_SetBits(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 /*| GPIO_Pin_11 | GPIO_Pin_10*/);
    GPIO_SetBits(GPIOG, GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8);
}

static void event_button_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

static void status_led_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOG, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_7);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOD, GPIO_Pin_14 | GPIO_Pin_15);

}


static void device_id_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
}


uint16_t get_device_gpio_status(void)
{
    return GPIO_ReadInputData(GPIOF) & 0x01ff;
}

static void platform_gpio_init(void)
{
    output_gpio_init();
    init_set_gpio();
    init_reset_gpio();

    led_init();
    input_gpio_init();
    charge_gpio_init();
    event_button_init();
    status_led_init();
    device_id_gpio_init();
}


uint8_t get_switch_state(void)
{
    return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11);
}

uint8_t get_event_buttton_state(void)
{
    return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6);
}

void hold_on_power(void)
{
    GPIO_SetBits(GPIOD, GPIO_Pin_10);
}

void release_power(void)
{
    GPIO_ResetBits(GPIOD, GPIO_Pin_10);
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

void led_mcu_power_ctrl(uint8_t on_off)
{
    if(on_off == MODULE_POWER_ON)
    {
        GPIO_ResetBits(GPIOE, GPIO_Pin_10);
    }
    else if(on_off == MODULE_POWER_OFF)
    {
        GPIO_SetBits(GPIOE, GPIO_Pin_10);
    }
}

void led_mcu_rst_ctrl(uint8_t on_off)
{
    if(on_off == MODULE_POWER_ON)
    {
        GPIO_SetBits(GPIOE, GPIO_Pin_11);
    }
    else if(on_off == MODULE_POWER_OFF)
    {
        GPIO_ResetBits(GPIOE, GPIO_Pin_11);
    }
}

void led_mcu_ctrl_init_and_power_on(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /*GPIO_E*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    led_mcu_rst_ctrl(MODULE_POWER_ON);
    led_mcu_power_ctrl(MODULE_POWER_ON);
}

void led_mcu_ctrl_deinit_and_power_off(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    led_mcu_rst_ctrl(MODULE_POWER_ON);
    led_mcu_power_ctrl(MODULE_POWER_ON);
    /*GPIO_E*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void beeper_on(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

void beeper_off(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
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

uint8_t get_module_5v_state(void)
{
    return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7);
}

uint8_t get_module_12v_state(void)
{
    return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8);
}

uint8_t get_module_24v_state(void)
{
    return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9);
}

uint8_t get_charge_gpio_value(void)
{
    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
}

uint8_t get_recharge_gpio_value(void)
{
    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6);
}

void camera_led_ctrl(uint8_t led, uint8_t on_off)
{
    if(on_off == MODULE_POWER_ON)
    {
        if(led & LED_CAMERA_FRONT)
        {
            GPIO_SetBits(GPIOG, GPIO_Pin_13);
        }
        if(led & LED_CAMERA_BACK)
        {
            GPIO_SetBits(GPIOG, GPIO_Pin_14);
        }
    }
    else if(on_off == MODULE_POWER_OFF)
    {
        if(led & LED_CAMERA_FRONT)
        {
            GPIO_ResetBits(GPIOG, GPIO_Pin_13);
        }
        if(led & LED_CAMERA_BACK)
        {
            GPIO_ResetBits(GPIOG, GPIO_Pin_14);
        }
    }

}


void door_ctrl(uint32_t door_id, uint8_t on_off)
{
    if(on_off == MODULE_POWER_ON)
    {
        if(door_id & DOOR_NO_ID_1)
        {
            GPIO_SetBits(GPIOG, GPIO_Pin_6);    //NV 24V
        }
        if(door_id & DOOR_NO_ID_2)
        {
//            GPIO_SetBits(GPIOG, GPIO_Pin_6);
        }
        if(door_id & DOOR_ID_1)
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_7);
        }
        if(door_id & DOOR_ID_2)
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_6);
        }
        if(door_id & DOOR_ID_3)
        {
            GPIO_SetBits(GPIOE, GPIO_Pin_6);
        }
        if(door_id & DOOR_ID_4)
        {
            GPIO_SetBits(GPIOE, GPIO_Pin_12);
        }
    }
    else if(on_off == MODULE_POWER_OFF)
    {
        if(door_id & DOOR_NO_ID_1)
        {
            GPIO_ResetBits(GPIOG, GPIO_Pin_6);  //NV 24V
        }
        if(door_id & DOOR_NO_ID_2)
        {
//            GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        }
        if(door_id & DOOR_ID_1)
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_7);
        }
        if(door_id & DOOR_ID_2)
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_6);
        }
        if(door_id & DOOR_ID_3)
        {
            GPIO_ResetBits(GPIOE, GPIO_Pin_6);
        }
        if(door_id & DOOR_ID_4)
        {
            GPIO_ResetBits(GPIOE, GPIO_Pin_12);
        }
    }
}


void led_ctrl(uint32_t leds, uint8_t on_off)
{
    if(on_off == MODULE_POWER_ON)
    {
        if(leds & POWER_LED_1)
        {
            GPIO_SetBits(platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_1].GPIOx, platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_1].GPIO_Pin);
        }
        if(leds & POWER_LED_2)
        {
            GPIO_SetBits(platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_2].GPIOx, platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_2].GPIO_Pin);
        }
        if(leds & POWER_LED_3)
        {
            GPIO_SetBits(platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_3].GPIOx, platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_3].GPIO_Pin);
        }
        if(leds & POWER_LED_4)
        {
            GPIO_SetBits(platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_4].GPIOx, platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_4].GPIO_Pin);
        }
        if(leds & POWER_LED_5)
        {
            GPIO_SetBits(platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_5].GPIOx, platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_5].GPIO_Pin);
        }
        if(leds & POWER_LED_6)
        {
            GPIO_SetBits(platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_6].GPIOx, platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_6].GPIO_Pin);
        }
    }
    if(on_off == MODULE_POWER_OFF)
    {
        if(leds & POWER_LED_1)
        {
            GPIO_ResetBits(platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_1].GPIOx, platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_1].GPIO_Pin);
        }
        if(leds & POWER_LED_2)
        {
            GPIO_ResetBits(platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_2].GPIOx, platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_2].GPIO_Pin);
        }
        if(leds & POWER_LED_3)
        {
            GPIO_ResetBits(platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_3].GPIOx, platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_3].GPIO_Pin);
        }
        if(leds & POWER_LED_4)
        {
            GPIO_ResetBits(platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_4].GPIOx, platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_4].GPIO_Pin);
        }
        if(leds & POWER_LED_5)
        {
            GPIO_ResetBits(platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_5].GPIOx, platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_5].GPIO_Pin);
        }
        if(leds & POWER_LED_6)
        {
            GPIO_ResetBits(platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_6].GPIOx, platform_gpio_pins[PLATFORM_GPIO_LED_CTRL_6].GPIO_Pin);
        }
    }
}

void led_ctrl_wifi_status(uint8_t status)
{
    if(status == LED_STATUS_OFF)
    {
        led_ctrl(POWER_LED_1 | POWER_LED_2, MODULE_POWER_OFF);
    }
    else if(status == LED_STATUS_OK)
    {
        led_ctrl(POWER_LED_1, MODULE_POWER_OFF);
        led_ctrl(POWER_LED_2, MODULE_POWER_ON);
    }
    else if(status == LED_STATUS_ERR)
    {
        led_ctrl(POWER_LED_2, MODULE_POWER_OFF);
        led_ctrl(POWER_LED_1, MODULE_POWER_ON);
    }
}

void led_ctrl_battery_status(uint8_t status)
{
    if(status == LED_STATUS_OFF)
    {
        led_ctrl(POWER_LED_3 | POWER_LED_4, MODULE_POWER_OFF);
    }
    else if(status == LED_STATUS_OK)
    {
        led_ctrl(POWER_LED_3, MODULE_POWER_OFF);
        led_ctrl(POWER_LED_4, MODULE_POWER_ON);
    }
    else if(status == LED_STATUS_ERR)
    {
        led_ctrl(POWER_LED_4, MODULE_POWER_OFF);
        led_ctrl(POWER_LED_3, MODULE_POWER_ON);
    }
}


void led_ctrl_trans_status(uint8_t status)
{
    if(status == LED_STATUS_OFF)
    {
        led_ctrl(POWER_LED_5 | POWER_LED_6, MODULE_POWER_OFF);
    }
    else if(status == LED_STATUS_OK)
    {
        led_ctrl(POWER_LED_5, MODULE_POWER_OFF);
        led_ctrl(POWER_LED_6, MODULE_POWER_ON);
    }
    else if(status == LED_STATUS_ERR)
    {
        led_ctrl(POWER_LED_6, MODULE_POWER_OFF);
        led_ctrl(POWER_LED_5, MODULE_POWER_ON);
    }
}


uint8_t get_camera_led_status(uint8_t led)
{
    if(led == LED_CAMERA_FRONT)
    {
        return GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_13);
    }
    if(led == LED_CAMERA_BACK)
    {
        return GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_14);
    }
    return 0;
}

uint8_t get_door_status(uint32_t door_id)
{
    if(door_id == DOOR_NO_ID_1)
    {
        return GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6);    //NV 24V
    }
    else if(door_id == DOOR_NO_ID_2)
    {
//            return GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6);
    }
    else if(door_id == DOOR_ID_1)
    {
        return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7);
    }
    else if(door_id == DOOR_ID_2)
    {
        return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6);
    }
    else if(door_id == DOOR_ID_3)
    {
        return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6);
    }
    else if(door_id == DOOR_ID_4)
    {
        return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12);
    }
    return 0;
}

void power_ctrl(uint32_t power_en, uint8_t on_off, uint8_t group)
{
    if(group == 1)
    {
        if(MODULE_POWER_ON == on_off)
        {
            if(power_en & POWER_5V_EN)
            {
                main_power_module_5v_ctrl(MODULE_POWER_ON);
            }


            if(power_en & POWER_12V_EN)
            {
                main_power_module_12v_ctrl(MODULE_POWER_ON);
            }
            if(power_en & POWER_24V_EN)
            {
                main_power_module_24v_ctrl(MODULE_POWER_ON);
            }


        //        if(power_en & POWER_LED_MCU)
        //        {
        //            MicoGpioOutputLow(MICO_GPIO_LED_MCU_POWER_EN);
        //        }
            if(power_en & POWER_LED_MCU)
            {
                led_mcu_power_ctrl(MODULE_POWER_ON);
            }
            if(power_en & LED_MCU_RST)
            {
                led_mcu_rst_ctrl(MODULE_POWER_ON);
            }


            if(power_en & POWER_CAMERA_FRONT_LED)
            {
                camera_led_ctrl(LED_CAMERA_FRONT, MODULE_POWER_ON);
            }
            if(power_en & POWER_CAMERA_BACK_LED)
            {
                camera_led_ctrl(LED_CAMERA_BACK, MODULE_POWER_ON);
            }

        //        if(power_en & POWER_CTRL_OUT)
        //        {
        //            MicoGpioOutputHigh(MICO_GPIO_PWR_CTRL_OUT);
        //        }

            if(power_en & POWER_VSYS_24V_NV)
            {
                door_ctrl(DOOR_NO_ID_1, MODULE_POWER_ON);
            }
            if(power_en & POWER_DOOR_CTRL)
            {
                door_ctrl(DOOR_NO_ID_2, MODULE_POWER_ON);
            }

            if(power_en & POWER_3V3_CARD_EN_1)
            {
                door_ctrl(DOOR_ID_1, MODULE_POWER_ON);
            }

            if(power_en & POWER_3V3_CARD_EN_2)
            {
                door_ctrl(DOOR_ID_2, MODULE_POWER_ON);
            }

            if(power_en & POWER_3V3_CARD_EN_3)
            {
                door_ctrl(DOOR_ID_3, MODULE_POWER_ON);
            }

            if(power_en & POWER_3V3_CARD_EN_4)
            {
                door_ctrl(DOOR_ID_4, MODULE_POWER_ON);
            }
        }
        else if(MODULE_POWER_OFF == on_off)
        {

            if(power_en & POWER_5V_EN)
            {
                main_power_module_5v_ctrl(MODULE_POWER_OFF);
            }

            if(power_en & POWER_12V_EN)
            {
                main_power_module_12v_ctrl(MODULE_POWER_OFF);
            }
            if(power_en & POWER_24V_EN)
            {
                main_power_module_24v_ctrl(MODULE_POWER_OFF);
            }


        //        if(power_en & POWER_LED_MCU)
        //        {
        //            MicoGpioOutputHigh(MICO_GPIO_LED_MCU_POWER_EN);
        //        }


        //        if(power_en & POWER_CTRL_OUT)
        //        {
        //            MicoGpioOutputLow(MICO_GPIO_PWR_CTRL_OUT);
        //        }

            if(power_en & POWER_LED_MCU)
            {
                led_mcu_power_ctrl(MODULE_POWER_OFF);
            }
            if(power_en & LED_MCU_RST)
            {
                led_mcu_rst_ctrl(MODULE_POWER_OFF);
            }
            if(power_en & POWER_CAMERA_FRONT_LED)
            {
                camera_led_ctrl(LED_CAMERA_FRONT, MODULE_POWER_OFF);
            }
            if(power_en & POWER_CAMERA_BACK_LED)
            {
                camera_led_ctrl(LED_CAMERA_BACK, MODULE_POWER_OFF);
            }

            if(power_en & POWER_VSYS_24V_NV)
            {
                door_ctrl(DOOR_NO_ID_1, MODULE_POWER_OFF);
            }
            if(power_en & POWER_DOOR_CTRL)
            {
                door_ctrl(DOOR_NO_ID_2, MODULE_POWER_OFF);
            }

            if(power_en & POWER_3V3_CARD_EN_1)
            {
                door_ctrl(DOOR_ID_1, MODULE_POWER_OFF);
            }

            if(power_en & POWER_3V3_CARD_EN_2)
            {
                door_ctrl(DOOR_ID_2, MODULE_POWER_OFF);
            }

            if(power_en & POWER_3V3_CARD_EN_3)
            {
                door_ctrl(DOOR_ID_3, MODULE_POWER_OFF);
            }

            if(power_en & POWER_3V3_CARD_EN_4)
            {
                door_ctrl(DOOR_ID_4, MODULE_POWER_OFF);
            }
        }
    }
    else if(group == 2)
    {
        /*
        */
    }
}


uint32_t get_module_power_state(uint32_t power_en)
{
    volatile uint32_t pin_state;

    pin_state = (uint32_t)0;

    if(power_en & POWER_5V_EN)
    {
        if(!get_module_5v_state())
        {
            pin_state |= POWER_5V_EN;
        }
    }

    if(power_en & POWER_12V_EN)
    {
        if(!get_module_12v_state())
        {
            pin_state |= POWER_12V_EN;
        }
    }

    if(power_en & POWER_24V_EN)
    {
        if(!get_module_24v_state())
        {
            pin_state |= POWER_24V_EN;
        }
    }


//    if(power_en & POWER_SYS_LED)
//    {
//        if(!MicoGpioInputGet(MICO_GPIO_SYS_LED))
//        {
//            pin_state |= POWER_SYS_LED;
//        }
//    }

//    if(power_en & POWER_LED_MCU)
//    {
//        if(!MicoGpioInputGet(MICO_GPIO_LED_MCU_POWER_EN))
//        {
//            pin_state |= POWER_LED_MCU;
//        }
//    }

    if(power_en & POWER_CAMERA_FRONT_LED)
    {
        if(get_camera_led_status(LED_CAMERA_FRONT))
        {
            pin_state |= POWER_CAMERA_FRONT_LED;
        }
    }
    if(power_en & POWER_CAMERA_BACK_LED)
    {
        if(get_camera_led_status(LED_CAMERA_BACK))
        {
            pin_state |= POWER_CAMERA_BACK_LED;
        }
    }

//    if(power_en & POWER_CTRL_OUT)
//    {
//        if(MicoGpioInputGet(MICO_GPIO_PWR_CTRL_OUT))
//        {
//            pin_state |= POWER_CTRL_OUT;
//        }
//    }




    if(power_en & POWER_VSYS_24V_NV)
    {
        if(!get_door_status(DOOR_NO_ID_1))
        {
            pin_state |= POWER_VSYS_24V_NV;
        }
    }

    if(power_en & POWER_DOOR_CTRL)
    {
        if(get_door_status(DOOR_NO_ID_2))
        {
            pin_state |= POWER_DOOR_CTRL;
        }
    }

    if(power_en & POWER_3V3_CARD_EN_1)
    {
        if(get_door_status(DOOR_ID_1))
        {
            pin_state |= POWER_3V3_CARD_EN_1;
        }
    }

    if(power_en & POWER_3V3_CARD_EN_2)
    {
        if(get_door_status(DOOR_ID_2))
        {
            pin_state |= POWER_3V3_CARD_EN_2;
        }
    }

    if(power_en & POWER_3V3_CARD_EN_3)
    {
        if(get_door_status(DOOR_ID_3))
        {
            pin_state |= POWER_3V3_CARD_EN_3;
        }
    }

    if(power_en & POWER_3V3_CARD_EN_4)
    {
        if(get_door_status(DOOR_ID_4))
        {
            pin_state |= POWER_3V3_CARD_EN_4;
        }
    }

    return pin_state;
}

uint16_t get_charge_recharge_init_state(void)
{
    uint8_t cnt = 0;
    uint16_t pre_state = 0;
    uint16_t state = 0;
    uint8_t charge_state = 0;
    uint8_t recharge_state = 0;
    uint16_t value = 0;
    recharge_state = get_recharge_gpio_value();
    pre_state &= 0xff00;
    pre_state += recharge_state;

    charge_state = get_charge_gpio_value();
    pre_state &= 0x00ff;
    pre_state += charge_state << 8;
    while(cnt <= 2)
    {
        delay_ms(5);
        recharge_state = get_recharge_gpio_value();
        state &= 0xffffff00;
        state += recharge_state;

        charge_state = get_charge_gpio_value();
        state &= 0xffff00ff;
        state += charge_state << 8;
        if(pre_state == state)
        {
            cnt++;
        }
        else
        {
            cnt = 0;
        }
        pre_state = state;
    }

    if(charge_state)
    {
        value |= STATE_IS_CHARGER_IN;
    }
    else
    {
        value &= ~STATE_IS_CHARGER_IN;
    }
    if(recharge_state)
    {
        value |= STATE_IS_RECHARGE_IN;
    }
    else
    {
        value &= ~STATE_IS_RECHARGE_IN;
    }
    return value;
}


#define RGB_LEDS_PWM_FREQUENCY     (1*1000)
#define RGB_LEDS_PWM_ARR            719
#define RGB_LEDS_PWM_PSC            (72*1000*1000) / (RGB_LEDS_PWM_FREQUENCY * (RGB_LEDS_PWM_ARR + 1))

void ir_led_pwm_ctrl(uint16_t duty)
{
    if(duty <= 100)
    {
        timer_1_ch1_pwm_init(719, 1, 719 * (100 - duty) / 100 );
    }
}

void rgb_leds_pwm_init(void)
{
    tim2_ch1_pwm_init(RGB_LEDS_PWM_ARR, RGB_LEDS_PWM_PSC, RGB_LEDS_PWM_ARR + 1);
    tim2_ch2_pwm_init(RGB_LEDS_PWM_ARR, RGB_LEDS_PWM_PSC, RGB_LEDS_PWM_ARR + 1);
    tim3_ch1_pwm_init(RGB_LEDS_PWM_ARR, RGB_LEDS_PWM_PSC, RGB_LEDS_PWM_ARR + 1);
}

void set_rgb_leds_r(uint8_t r)
{
    uint16_t rr = r * 10;
    rr = 100 * rr / 255;
    rr = (rr + 5) / 10;
    change_tim2_ch1_pwm_duty(RGB_LEDS_PWM_ARR * (100 - rr) / 100 + 1);
}

void set_rgb_leds_g(uint8_t g)
{
    uint16_t gg = g * 10;
    gg = 100 * gg / 255;
    gg = (gg + 5) / 10;
    change_tim2_ch2_pwm_duty(RGB_LEDS_PWM_ARR * (100 - gg) / 100 + 1);
}

void set_rgb_leds_b(uint8_t b)
{
    uint16_t bb = b * 10;
    bb = 100 * bb / 255;
    bb = (bb + 5) / 10;
    change_tim3_ch1_pwm_duty(RGB_LEDS_PWM_ARR * (100 - bb) / 100 + 1);
}

uint32_t test_hardware_version = 0;
uint16_t device_id = 0;
void hardware_init(void)
{
    platform_gpio_init();
    bat_uart_init();
    init_can1();
    ir_led_pwm_ctrl(20);
    rgb_leds_pwm_init();
    test_hardware_version = get_hardware_version();
    device_id = get_device_id();
}

void user_param_init(void)
{
    sys_status->sys_status = get_charge_recharge_init_state();
}

