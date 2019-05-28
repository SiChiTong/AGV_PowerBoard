#ifndef __USER_PLATFORM_H__
#define __USER_PLATFORM_H__
#include "stm32f10x.h"
#include "ucos_ii.h"

#define SERIAL_LED_RGB          1
#define SERIAL_LED_ONE_WIRE     2
#define SERIAL_LED_TYPE         SERIAL_LED_ONE_WIRE

typedef enum
{
#if 0
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
#endif

//    PLATFORM_GPIO_12V_PAD_EN,

//    PLATFORM_GPIO_12V_EXTEND_EN,
//    PLATFORM_GPIO_12V_X86_EN,
//    PLATFORM_GPIO_12V_NV_EN,

    PLATFORM_GPIO_PWR_NV,
    PLATFORM_GPIO_PWR_PAD,
    PLATFORM_GPIO_PWR_X86,
    PLATFORM_GPIO_PWR_RESERVE,

    PLATFORM_GPIO_5V_EN,
    PLATFORM_GPIO_12V_EN,
    PLATFORM_GPIO_24V_EN,

    PLATFORM_GPIO_PWRKEY,

    PLATFORM_GPIO_IRLED_PWM,
    PLATFORM_GPIO_SYS_LED,

    PLATFORM_GPIO_VSYS_24V_NV_EN,

    PLATFORM_GPIO_LED_MCU_POWER_EN,
    PLATFORM_GPIO_LED_MCU_RESET,

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

    PLATFORM_GPIO_SERIAL_LED_FRONT_LEFT,
    PLATFORM_GPIO_SERIAL_LED_FRONT_RIGHT,
    PLATFORM_GPIO_SERIAL_LED_BACK_LEFT,
    PLATFORM_GPIO_SERIAL_LED_BACK_RIGHT,
    PLATFORM_GPIO_SERIAL_LED_EYES,

    PLATFORM_GPIO_EVENT_BUTTON,

    PLATFORM_GPIO_LED_CTRL_1,
    PLATFORM_GPIO_LED_CTRL_2,
    PLATFORM_GPIO_LED_CTRL_3,
    PLATFORM_GPIO_LED_CTRL_4,
    PLATFORM_GPIO_LED_CTRL_5,
    PLATFORM_GPIO_LED_CTRL_6,

    PLATFORM_GPIO_S_1,
    PLATFORM_GPIO_S_2,
    PLATFORM_GPIO_S_3,
    PLATFORM_GPIO_S_4,
    PLATFORM_GPIO_S_5,
    PLATFORM_GPIO_S_6,
    PLATFORM_GPIO_S_7,
    PLATFORM_GPIO_S_8,
    PLATFORM_GPIO_S_9,

    PLATFORM_GPIO_HEAD_CAMERA_LED,

    PLATFORM_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    PLATFORM_GPIO_NONE,
} platform_gpio_e;




/*********** power control group 1 *************/
//#define POWER_5V_MOTOR             0x00000001
#define POWER_5V_RECHARGE          0x00000002
#define POWER_5V_SENSOR_BOARD      0x00000004
    //POWER_5V_SWITCH           = 0x00000008,
//#define POWER_5V_ROUTER            0x00000010
#define POWER_5V_EN                0x00000020

#define POWER_12V_PAD              0x00000040
//#define POWER_12V_2_1_PA           0x00000080
//#define POWER_12V_EXTEND           0x00000100
#define POWER_12V_X86              0x00000200
#define POWER_12V_NV               0x00000400
#define POWER_12V_EN               0x00000800

#define POWER_24V_EN               0x00001000
//#define POWER_24V_PRINTER          0x00002000
#define POWER_24V_EXTEND           0x00004000
#define POWER_VSYS_24V_NV          0x00008000

//#define POWER_485                  0x00010000
#define POWER_SYS_LED              0x00020000
#define POWER_RECHARGE_LED         0x00040000
#define POWER_SLAM                 0x00080000

#define POWER_LED_MCU              0x00100000
#define POWER_CHARGE_FAN           0x00200000
    //POWER_POLE_MOTOR          = 0x00400000,
    //POWER_5V_KEYPAD           = 0x00800000,
#define LED_MCU_RST                0x00400000

#define POWER_CAMERA_FRONT_LED     0x01000000
#define POWER_CAMERA_BACK_LED      0x02000000
#define POWER_CTRL_OUT             0x04000000
#define POWER_DOOR_CTRL            0x08000000

#define POWER_3V3_CARD_EN_1        0x10000000
#define POWER_3V3_CARD_EN_2        0x20000000
#define POWER_3V3_CARD_EN_3        0x40000000
#define POWER_3V3_CARD_EN_4        0x80000000


#define POWER_ALL                  0xFFFFFFFF





/*********** power control group 2 *************/
#define POWER_LED_1                 0x00000001
#define POWER_LED_2                 0x00000002
#define POWER_LED_3                 0x00000004
#define POWER_LED_4                 0x00000008
#define POWER_LED_5                 0x00000010
#define POWER_LED_6                 0x00000020
#define POWER_LED_7                 0x00000040
#define POWER_LED_8                 0x00000080

#define POWER_MOTOR_MCU             0x00000100
#define POWER_HEAD_CAMERA_LED       0x00000200

//#define MODULE_POWER_ON     1
//#define MODULE_POWER_OFF    0

typedef enum
{
    MODULE_POWER_OFF = 0,
    MODULE_POWER_ON
}module_power_state_e;


typedef enum
{
    LED_MIN = 0,
    LED_WIFI = 1,
    LED_TRANS,
    LED_BATTERY,
    LED_MAX,
}status_led_e;

typedef enum
{
    LED_STATUS_MIN = 0,
    LED_STATUS_OFF = 1,
    LED_STATUS_OK ,
    LED_STATUS_ERR,
    LED_STATUS_WARN,
    LED_STATUS_MAX,
}led_ctrl_status_e;




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


#define                 REMOTE_DEVICE_POWER_SHUTDOWN    1
#define                 REMOTE_DEVICE_POWER_REBOOT      2
typedef struct
{
    uint16_t sys_status;    //œÚ«∞ºÊ»›
    uint8_t power_state;
    uint8_t is_booting_up_finished;
    uint8_t is_shutting_down_finished;
    uint8_t ir_led_pwm_duty;
    uint8_t remote_device_power_ctrl;
    uint8_t event_button_state;
}sys_status_t;

extern sys_status_t *sys_status;
extern const platform_gpio_t platform_gpio_pins[];

uint32_t get_tick(void);
void mcu_restart(void);

void hardware_init(void);
void user_param_init(void);
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
uint8_t get_event_buttton_state(void);

void ir_led_pwm_ctrl(uint16_t duty);
void beeper_on(void);
void beeper_off(void);

void power_ctrl(uint32_t power_en, uint8_t on_off, uint8_t group);
uint32_t get_module_power_state(uint32_t power_en);
void led_mcu_ctrl_init_and_power_on(void);
void led_mcu_ctrl_deinit_and_power_off(void);


void set_rgb_leds_r(uint8_t r);
void set_rgb_leds_g(uint8_t g);
void set_rgb_leds_b(uint8_t b);

void led_ctrl_wifi_status(uint8_t status);
void led_ctrl_battery_status(uint8_t status);
void led_ctrl_trans_status(uint8_t status);

uint16_t get_device_gpio_status(void);

#endif
