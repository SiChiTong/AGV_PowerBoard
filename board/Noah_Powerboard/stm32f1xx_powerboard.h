/*
 *  Author: Adam Huang
 *  Date:2016/6/4
 */
#ifndef __STM32F1XX_POWERBOARD_H
#define __STM32F1XX_POWERBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"


typedef enum
{
    POWER_OFF = 0,
    POWER_ON = 1
} power_on_off_type_def;

typedef enum {
    CONTROL_RELEASE = 0,
    CONTROL_HOLD = 1
} control_signal_type_def;

typedef enum
{
    LED_SYS = 0,

} led_type_def;

typedef enum
{
    SWITCH_USER = 0,
} switch_type_def;


typedef enum
{
#if 0
    POWER_SWITCH          = 0x00000001,
    POWER_DLP             = 0x00000002,
    POWER_X86             = 0x00000004,
    POWER_PAD             = 0x00000008,
    POWER_5V_EN           = 0x00000010,
    POWER_12V_EN          = 0x00000020,
    POWER_24V_EN          = 0x00000040,
    POWER_MOTOR           = 0x00000080,
    POWER_SENSOR          = 0x00000100,
    POWER_LEDS            = 0x00000200,
    POWER_5V_RES          = 0x00000400,
    POWER_12V_ROUTOR      = 0x00000800,
    POWER_2_1_PA          = 0x00001000,
    POWER_NV              = 0x00002000,
    POWER_PRINTER         = 0x00004000,
    POWER_DYP             = 0x00008000,
    POWER_12V_RES         = 0x00010000,
    POWER_24V_RES         = 0x00020000,
    POWER_BAT_NV          = 0x00040000,
    POWER_AIUI            = 0x00080000,
    POWER_5V_ROUTOR       = 0x00100000,
    POWER_ALL             = 0x001FFFFF,
#endif

    POWER_5V_MOTOR            = 0x00000001,
    POWER_5V_RECHARGE         = 0x00000002,
    POWER_5V_SENSOR_BOARD     = 0x00000004,
    //POWER_5V_SWITCH           = 0x00000008,
    POWER_5V_ROUTER           = 0x00000010,
    POWER_5V_EN               = 0x00000020,

    POWER_12V_PAD             = 0x00000040,
    POWER_12V_2_1_PA          = 0x00000080,
    POWER_12V_EXTEND          = 0x00000100,
    POWER_12V_X86             = 0x00000200,
    POWER_12V_NV              = 0x00000400,
    POWER_12V_EN              = 0x00000800,

    POWER_24V_EN              = 0x00001000,
    POWER_24V_PRINTER         = 0x00002000,
    POWER_24V_EXTEND          = 0x00004000,
    POWER_VSYS_24V_NV         = 0x00008000,

    POWER_485                 = 0x00010000,
    POWER_SYS_LED             = 0x00020000,
    POWER_RECHARGE_LED        = 0x00040000,
    POWER_SLAM                = 0x00080000,

    POWER_LED_MCU             = 0x00100000,
    POWER_CHARGE_FAN          = 0x00200000,
    //POWER_POLE_MOTOR          = 0x00400000,
    //POWER_5V_KEYPAD           = 0x00800000,

    POWER_CAMERA_FRONT_LED    = 0x01000000,
    POWER_CAMERA_BACK_LED     = 0x02000000,
    POWER_CTRL_OUT            = 0x04000000,
    POWER_DOOR_CTRL           = 0x08000000,

    POWER_3V3_CARD_EN_1       = 0x10000000,
    POWER_3V3_CARD_EN_2       = 0x20000000,
    POWER_3V3_CARD_EN_3       = 0x40000000,
    POWER_3V3_CARD_EN_4       = 0x80000000,


    POWER_ALL                 = 0xFFFFFFFF,


} power_enable_type_def;

typedef enum
{
    POWER_CON_DLP = 0x01,
    POWER_CON_PAD = 0x02,
    POWER_CON_X86 = 0x04,
    POWER_CON_NV  = 0x08,
    POWER_CON_RES = 0x10,
    POWER_CON_ALL = 0x1F
} power_control_type_def;


uint32_t get_switch_state(switch_type_def Switch);


void power_ctrl(uint32_t power_en, power_on_off_type_def OnOff);
uint32_t get_module_power_state(power_enable_type_def power_en);
void module_signal_ctrl(power_control_type_def power_control, control_signal_type_def is_hold);
void en_led_mcu(void);

extern void enter_sleep_mode( void );

extern void wake_up_from_sleep_mode( void );

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1XX_POWERBOARD_H */

