#ifndef __USER_PLATFORM_H__
#define __USER_PLATFORM_H__
#include "stm32f10x.h"
#include "ucos_ii.h"


typedef enum
{
    PLATFORM_GPIO_CAN_RX,
    PLATFORM_GPIO_CAN_TX,
    PLATFORM_GPIO_CAN_STB,

    PLATFORM_GPIO_SPI_NSS,
    PLATFORM_GPIO_SPI_SCK,
    PLATFORM_GPIO_SPI_MISO,
    PLATFORM_GPIO_SPI_MOSI,

    PLATFORM_GPIO_UART5_TX,
    PLATFORM_GPIO_UART5_RX,

    PLATFORM_GPIO_UART4_TX,
    PLATFORM_GPIO_UART4_RX,

    PLATFORM_GPIO_UART3_TX,
    PLATFORM_GPIO_UART3_RX,

    PLATFORM_GPIO_UART2_TX,
    PLATFORM_GPIO_UART2_RX,

    PLATFORM_GPIO_UART1_TX,
    PLATFORM_GPIO_UART1_RX,

    PLATFORM_GPIO_I2C_SCL,
    PLATFORM_GPIO_I2C_SDA,

    PLATFORM_GPIO_5V_MOTOR_EN,
    PLATFORM_GPIO_5V_RECHARGE_EN,
    PLATFORM_GPIO_5V_SENSOR_BOARD_EN,
    PLATFORM_GPIO_5V_SWITCH_EN,
    PLATFORM_GPIO_12V_PAD_EN,
    PLATFORM_GPIO_12V_2_1_PA_EN,
    PLATFORM_GPIO_12V_EXTEND_EN,
    PLATFORM_GPIO_12V_X86_EN,
    PLATFORM_GPIO_12V_NV_EN,


    PLATFORM_GPIO_PWR_NV,
    PLATFORM_GPIO_PWR_PAD,
    PLATFORM_GPIO_PWR_X86,
    PLATFORM_GPIO_PWR_RESERVE,

    PLATFORM_GPIO_5V_EN,
    PLATFORM_GPIO_12V_EN,
    PLATFORM_GPIO_24V_EN,

    PLATFORM_GPIO_CHARGE_IN,
    PLATFORM_GPIO_RECHARGE_IN,

    PLATFORM_GPIO_SWITCH_EN,
    PLATFORM_GPIO_SWITCH_SEL0,
    PLATFORM_GPIO_SWITCH_SEL1,
    PLATFORM_GPIO_SWITCH_SEL2,
    PLATFORM_GPIO_SWITCH_SEL3,

    PLATFORM_GPIO_PWRKEY,
    PLATFORM_GPIO_485_EN,
    PLATFORM_GPIO_IRLED_PWM,
    PLATFORM_GPIO_SYS_LED,
    PLATFORM_GPIO_RECHARGE_LED,

    PLATFORM_GPIO_5V_ROUTER_EN,
    PLATFORM_GPIO_VSYS_24V_NV_EN,
    PLATFORM_GPIO_SLAM_EN,
    PLATFORM_GPIO_24V_PRINTER_EN,
    PLATFORM_GPIO_24V_EXTEND_EN,


    PLATFORM_GPIO_CHARGE_ADC,
    PLATFORM_GPIO_BATIN_ADC,
    PLATFORM_GPIO_VBUS_ADC,
    PLATFORM_GPIO_BAT_MOTOR_ADC,
    PLATFORM_GPIO_12V_2_1_PA_ADC,
    PLATFORM_GPIO_12V_PAD_ADC,
    PLATFORM_GPIO_24V_PRINTER_ADC,
    PLATFORM_GPIO_12V_X86_ADC,
    PLATFORM_GPIO_5V_RES1_ADC,
    PLATFORM_GPIO_12V_NV_ADC,
    PLATFORM_GPIO_IRLED_ADC,
    PLATFORM_GPIO_5V_MOTOR_ADC,

    PLATFORM_GPIO_MULTI_CHANNEL_ADC,
    PLATFORM_GPIO_VSYS_24V_NV_ADC,
    PLATFORM_GPIO_48V_EXTEND_ADC,
    PLATFORM_GPIO_5V_POLE_MOTOR_ADC,
    PLATFORM_GPIO_12V_EXTEND_ADC,
    PLATFORM_GPIO_RECHARGE_ADC,
    PLATFORM_GPIO_24V_EXTEND_ADC,
    PLATFORM_GPIO_5V_LEDS_ADC,
    PLATFORM_GPIO_24V_SLAM_ADC,

    PLATFORM_GPIO_LED_MCU_POWER_EN,
    PLATFORM_GPIO_LED_MCU_RESET,

    PLATFORM_GPIO_CHARGE_FAN_CTRL,

    PLATFORM_GPIO_FAN_1_CTRL,
    PLATFORM_GPIO_FAN_2_CTRL,
    PLATFORM_GPIO_FAN_3_CTRL,

    PLATFORM_GPIO_5V_KEYPAD_EN,

    PLATFORM_GPIO_CAMERA_BACK_LED_EN,
    PLATFORM_GPIO_CAMERA_FRONT_LED_EN,
    PLATFORM_GPIO_PWR_CTRL_OUT,
    PLATFORM_GPIO_PWR_CTRL_IN,
    PLATFORM_GPIO_3_3V_DOOR_CTRL,

    PLATFORM_GPIO_HW_VERSION_ID_0,
    PLATFORM_GPIO_HW_VERSION_ID_1,
    PLATFORM_GPIO_3V3_CARD_EN_1,
    PLATFORM_GPIO_3V3_CARD_EN_2,
    PLATFORM_GPIO_3V3_CARD_EN_3,
    PLATFORM_GPIO_3V3_CARD_EN_4,

    PLATFORM_GPIO_FAN_12V_DC_CTRL,
    PLATFORM_GPIO_BEEPER_CTRL,

    PLATFORM_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    PLATFORM_GPIO_NONE,
} platform_gpio_e;



#define POWER_5V_MOTOR             0x00000001
#define POWER_5V_RECHARGE          0x00000002
#define POWER_5V_SENSOR_BOARD      0x00000004
    //POWER_5V_SWITCH           = 0x00000008,
#define POWER_5V_ROUTER            0x00000010
#define POWER_5V_EN                0x00000020

#define POWER_12V_PAD              0x00000040
#define POWER_12V_2_1_PA           0x00000080
#define POWER_12V_EXTEND           0x00000100
#define POWER_12V_X86              0x00000200
#define POWER_12V_NV               0x00000400
#define POWER_12V_EN               0x00000800

#define POWER_24V_EN               0x00001000
#define POWER_24V_PRINTER          0x00002000
#define POWER_24V_EXTEND           0x00004000
#define POWER_VSYS_24V_NV          0x00008000

#define POWER_485                  0x00010000
#define POWER_SYS_LED              0x00020000
#define POWER_RECHARGE_LED         0x00040000
#define POWER_SLAM                 0x00080000

#define POWER_LED_MCU              0x00100000
#define POWER_CHARGE_FAN           0x00200000
    //POWER_POLE_MOTOR          = 0x00400000,
    //POWER_5V_KEYPAD           = 0x00800000,

#define POWER_CAMERA_FRONT_LED     0x01000000
#define POWER_CAMERA_BACK_LED      0x02000000
#define POWER_CTRL_OUT             0x04000000
#define POWER_DOOR_CTRL            0x08000000

#define POWER_3V3_CARD_EN_1        0x10000000
#define POWER_3V3_CARD_EN_2        0x20000000
#define POWER_3V3_CARD_EN_3        0x40000000
#define POWER_3V3_CARD_EN_4        0x80000000


#define POWER_ALL                  0xFFFFFFFF


//#define MODULE_POWER_ON     1
//#define MODULE_POWER_OFF    0

typedef enum
{
    MODULE_POWER_OFF = 0,
    MODULE_POWER_ON
}module_power_state_e;

typedef enum
{
    LED_CAMERA_FRONT = 0x01,
    LED_CAMERA_BACK = 0x02,
}led_e;

typedef enum
{
    DOOR_NO_ID_1 = 0x01,
    DOOR_NO_ID_2 = 0x02,
    DOOR_NO_ID_3 = 0x04,
    DOOR_ID_1 = 0x10,
    DOOR_ID_2 = 0x20,
    DOOR_ID_3 = 0x40,
    DOOR_ID_4 = 0x800
}door_e;


typedef struct
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;

}platform_gpio_t;





#define SYS_POWER_STATE_BOOTING_UP      1
#define SYS_POWER_STATE_ON              2
#define SYS_POWER_STATE_SHUTTING_DOWN   3
#define SYS_POWER_STATE_OFF             0


#define                 STATE_RUN_BITS     0x0F
#define                 STATE_POWER_OFF    0x00
#define                 STATE_IS_POWER_ON  0x01
#define                 STATE_POWER_ON     0x02
#define                 STATE_IS_POWER_OFF 0x03
//#define                 STATE_ERROR        0x04

#define                 STATE_IS_CHARGING       0x10
#define                 STATE_IS_LOW_POWER      0x20
#define                 STATE_IS_AUTO_UPLOAD    0x40
#define                 STATE_IS_CHARGER_IN     0x80
#define                 STATE_IS_RECHARGE_IN    0x0100
#define                 SYSTEM_IS_SLEEP         0x00 //set 0x00 to no use

typedef struct
{
    uint16_t sys_status;    //œÚ«∞ºÊ»›
    uint8_t power_state;
    uint8_t is_booting_up_finished;
    uint8_t is_shutting_down_finished;
    uint8_t charge_state;
    uint8_t recharge_state;
    uint8_t ir_led_pwm_duty;
}sys_status_t;

extern sys_status_t *sys_status;

uint32_t get_tick(void);
void mcu_restart(void);

void hardware_init(void);
void hold_on_power(void);
void release_power(void);
void main_power_module_5v_ctrl(uint8_t on_off);
void main_power_module_12v_ctrl(uint8_t on_off);
void main_power_module_24v_ctrl(uint8_t on_off);
void x86_power_signal_ctrl(uint8_t on_off);
void rk_power_signal_ctrl(uint8_t on_off);
uint8_t get_switch_state(void);
uint8_t get_charge_gpio_value(void);
uint8_t get_recharge_gpio_value(void);

void ir_led_pwm_ctrl(uint16_t duty);
void beeper_on(void);
void beeper_off(void);

void power_ctrl(uint32_t power_en, uint8_t on_off);
uint32_t get_module_power_state(uint32_t power_en);

#endif
