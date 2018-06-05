/**
******************************************************************************
* @file    platform.h
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provides all MICO Peripherals defined for current platform.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy 
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/ 



#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
   
/******************************************************
 *                   Enumerations
 ******************************************************/

/*
POWERBOARD V1.2 platform pin definitions ...
+-------------------------------------------------------------------------+
| Enum ID       |Pin | STM32| Peripheral  |    Board     |   Peripheral   |
|               | #  | Port | Available   |  Connection  |     Alias      |
|---------------+----+------+-------------+--------------+----------------|
|               | 1  | VCC  |             |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 2  | C 13 |             |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 3  | C 14 |             |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 4  | C 15 |             |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 5  | D  0 |             |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 6  | D  1 |             |              |                |
+---------------+----+--------------------+--------------+----------------+
|               | 7  | NRST |             |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_UART1_TX | 29 | B 10 | USART3_TX   |              | MICO_UART_3_TX |
|               |    |      | GPIO        |              |                |
|               |    |      | I2C2_SCL    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_UART1_RX | 30 | B 11 | USART3_RX   |              | MICO_UART_3_RX |
|               |    |      | GPIO        |              |                |
|               |    |      | I2C2_SDA    |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_UART2_TX | 42 | A  9 | USART1_TX   |              | MICO_UART_1_TX |
|               |    |      | GPIO        |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_UART2_RX | 43 | B 10 | USART1_RX   |              | MICO_UART_1_RX |
|               |    |      | GPIO        |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_SYS_LED  | 40 | C  9 | GPIO        |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_SWITCH   | 37 | C  6 | GPIO        |              |                |
+---------------+----+--------------------+--------------+----------------+
| MICO_GPIO_1   | 8  | C  0 | ADC12_IN10  |              |  ADC_2.1_PA    |
|               |    |      | GPIO        |              |                |
|---------------+----+------+-------------+--------------+----------------|
| MICO_GPIO_2   | 9  | C  1 | ADC12_IN11  |              |     ADC_PAD    |
|               |    |      | GPIO        |              |                |
|               |    |      |             |              |                |
+---------------+----+------+-------------+--------------+----------------+
*
*/
  
#define MICO_UNUSED 0xFF

#define HW_V2_1

#define NOAH_HW_V0_1

typedef enum
{    
    MICO_GPIO_CAN_RX,
    MICO_GPIO_CAN_TX,
    MICO_GPIO_CAN_STB,

    MICO_GPIO_SPI_NSS,
    MICO_GPIO_SPI_SCK,
    MICO_GPIO_SPI_MISO,
    MICO_GPIO_SPI_MOSI,
    
    MICO_GPIO_UART5_TX,
    MICO_GPIO_UART5_RX,
    
    MICO_GPIO_UART4_TX,
    MICO_GPIO_UART4_RX,
    
    MICO_GPIO_UART3_TX,
    MICO_GPIO_UART3_RX,
   
    MICO_GPIO_UART2_TX,
    MICO_GPIO_UART2_RX,
  
    MICO_GPIO_UART1_TX,
    MICO_GPIO_UART1_RX,
    
    MICO_GPIO_I2C_SCL,
    MICO_GPIO_I2C_SDA,
    
///////////////////////////////////////////////////////////////   
    
    
    MICO_GPIO_5V_MOTOR_EN,          //new
    MICO_GPIO_5V_RECHARGE_EN,       //new
    MICO_GPIO_5V_SENSOR_BOARD_EN,   //new
    //MICO_GPIO_5V_SWITCH_EN,         //new  /////////////////
    MICO_GPIO_12V_PAD_EN,           //new
    MICO_GPIO_12V_2_1_PA_EN,        //new
    MICO_GPIO_12V_EXTEND_EN,        //new
    MICO_GPIO_12V_X86_EN,           //new
    MICO_GPIO_12V_NV_EN,            //new
    
    
    MICO_GPIO_PWR_NV,
    MICO_GPIO_PWR_PAD,
    MICO_GPIO_PWR_X86,
    MICO_GPIO_PWR_RESERVE,
   
    MICO_GPIO_5V_EN,
    MICO_GPIO_12V_EN,
    MICO_GPIO_24V_EN,
    
    MICO_GPIO_CHARGE_IN,
    MICO_GPIO_RECHARGE_IN,
    
    MICO_GPIO_SWITCH_EN,
    MICO_GPIO_SWITCH_SEL0,
    MICO_GPIO_SWITCH_SEL1,
    MICO_GPIO_SWITCH_SEL2,
    MICO_GPIO_SWITCH_SEL3,
    
    MICO_GPIO_PWRKEY,           //new
    MICO_GPIO_485_EN,           //new
    MICO_GPIO_IRLED_PWM,        //new
    MICO_GPIO_SYS_LED,          //new
    MICO_GPIO_RECHARGE_LED,     //new

    MICO_GPIO_5V_ROUTER_EN,     //new
    MICO_GPIO_VSYS_24V_NV_EN,   //new
    MICO_GPIO_SLAM_EN,          //new
    MICO_GPIO_24V_PRINTER_EN,   //new
    MICO_GPIO_24V_EXTEND_EN,    //new
    

#if 0    
    MICO_GPIO_FRONT_LEFT_LED,
    MICO_GPIO_FRONT_RIGHT_LED,
    MICO_GPIO_BACK_LEFT_LED,
    MICO_GPIO_BACK_RIGHT_LED,
    MICO_GPIO_LEFT_EYE_LED,
    MICO_GPIO_RIGHT_EYE_LED,
#endif  
    

    MICO_GPIO_CHARGE_ADC,           //new     
    MICO_GPIO_BATIN_ADC,            //new
    MICO_GPIO_VBUS_ADC,             //new
    MICO_GPIO_BAT_MOTOR_ADC,        //new
    MICO_GPIO_12V_2_1_PA_ADC,           //new
    MICO_GPIO_12V_PAD_ADC,              //new
    MICO_GPIO_24V_PRINTER_ADC,          //new
    MICO_GPIO_12V_X86_ADC,              //new
    MICO_GPIO_5V_RES1_ADC,          //new
    MICO_GPIO_12V_NV_ADC,           //new
    MICO_GPIO_IRLED_ADC,            //new
    MICO_GPIO_5V_MOTOR_ADC,            //new

    MICO_GPIO_MULTI_CHANNEL_ADC,    //new
    MICO_GPIO_VSYS_24V_NV_ADC,      //new
    MICO_GPIO_48V_EXTEND_ADC,       //new
    MICO_GPIO_5V_POLE_MOTOR_ADC,    //V0.1
    MICO_GPIO_12V_EXTEND_ADC,       //new
    MICO_GPIO_RECHARGE_ADC,         //new
    MICO_GPIO_24V_EXTEND_ADC,       //new
    MICO_GPIO_5V_LEDS_ADC,           //new
    MICO_GPIO_24V_SLAM_ADC,         //new
    
    MICO_GPIO_LED_MCU_POWER_EN,//V0.1
    MICO_GPIO_LED_MCU_RESET,//V0.1
    
    MICO_GPIO_CHARGE_FAN_CTRL,
    
    MICO_GPIO_FAN_1_CTRL,
    MICO_GPIO_FAN_2_CTRL,
    MICO_GPIO_FAN_3_CTRL,
    //MICO_GPIO_5V_POLE_MOTOR_EN,////////
    //MICO_GPIO_5V_KEYPAD_EN,///////
    
    MICO_GPIO_CAMERA_BACK_LED_EN,
    MICO_GPIO_CAMERA_FRONT_LED_EN,
    MICO_GPIO_PWR_CTRL_OUT,
    MICO_GPIO_PWR_CTRL_IN,
    MICO_GPIO_3_3V_DOOR_CTRL,
    
    MICO_GPIO_HW_VERSION_ID_0,
    MICO_GPIO_HW_VERSION_ID_1,
    MICO_GPIO_3V3_CARD_EN_1,
    MICO_GPIO_3V3_CARD_EN_2,
    MICO_GPIO_3V3_CARD_EN_3,
    MICO_GPIO_3V3_CARD_EN_4,
    
    MICO_GPIO_FAN_12V_DC_CTRL,
    
    
    
    
    

    MICO_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    MICO_GPIO_NONE,
} mico_gpio_t;

typedef enum
{
    MICO_SPI_1,
    MICO_SPI_MAX, /* Denotes the total number of SPI port aliases. Not a valid SPI alias */
    MICO_SPI_NONE,
} mico_spi_t;

typedef enum
{
    MICO_I2C_1,
    MICO_I2C_MAX, /* Denotes the total number of I2C port aliases. Not a valid I2C alias */
    MICO_I2C_NONE,
} mico_i2c_t;

typedef enum
{
    MICO_PWM_IRLED,
    MICO_PWM_1,
    MICO_PWM_2,
    MICO_PWM_3,
    MICO_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
    MICO_PWM_NONE,
} mico_pwm_t;

typedef enum
{
    /* following are adc1 channels */
#ifdef     ADC_TEST_TMP
    MICO_ADC_5V_RES1,
    MICO_ADC_12V_RES2,
    MICO_ADC_BAT_NV,
    MICO_ADC_12V_NV,
    MICO_ADC_ROUTER_12V,
    MICO_ADC_DYP,
    MICO_ADC_SENSOR,
    MICO_ADC_DLP,
    MICO_ADC_MOTOR,
    MICO_ADC_24V_RES1,
    MICO_ADC_2_1_PA,
    MICO_ADC_PAD,
    MICO_ADC_PRINTER,
    MICO_ADC_X86,
    MICO_ADC_IRLED,
    MICO_ADC_LEDS,
#else
   
    MICO_ADC_5V_RESERVE_C,      //0
    
    //MICO_ADC_24V_NV_C,       //2
    //MICO_ADC_12V_NV_C,       //3
    MICO_ADC_5V_POLE_MOTOR_C, //1
    MICO_ADC_48V_EXTEND_C,  //4
    MICO_ADC_12V_EXTEND_C,  //5
    MICO_ADC_RECHARGE_C,    //6
    MICO_ADC_5V_MOTOR_C,       //8
    MICO_ADC_24V_SLAM_C,        //9 
    MICO_ADC_12V_2_1_PA_C,      //10
    
    MICO_ADC_12V_PAD_C,         //11
    MICO_ADC_24V_PRINTER_C,     //12
    MICO_ADC_12V_X86_C,         //13
    MICO_ADC_IRLED_C,       //14
    MICO_ADC_5V_LEDS_C,     //15
#endif  
    /* following are adc3 channels */
#ifdef     ADC_TEST_TMP
    MICO_ADC_CHARGE,
    MICO_ADC_BATIN,
    MICO_ADC_VBUS,
    MICO_ADC_BAT_MOTOR,
    MICO_ADC_SWITCH,
#else   
    
    
    MICO_ADC_24V_EXTEND_C,  //2
    
    MICO_ADC_CHARGE_C,      //channal 4
    MICO_ADC_BATIN_C,       //5
    MICO_ADC_VBUS_C,        //6
    MICO_ADC_BAT_MOTOR_C,   //7
    MICO_ADC_MULTI_CHANNAL, //8   
#endif  
    /* begin of virtual adc */
    
#ifdef     ADC_TEST_TMP
    MICO_ADC_24V_TS,
    MICO_ADC_12V_TS,
    MICO_ADC_5V_TS,
    MICO_ADC_AIR_TS,
    MICO_ADC_24V_ALL,
    MICO_ADC_12V_ALL,
    MICO_ADC_5V_ALL,
    MICO_ADC_VDET_24V,
    MICO_ADC_VDET_12V,
    MICO_ADC_VDET_5V,
    MICO_ADC_VDET_BAT,
    MICO_ADC_AIUI,
    MICO_ADC_ROUTER_5V,
#else 
       
    MICO_ADC_24V_TS,            //0
    MICO_ADC_12V_TS,            //1   
    MICO_ADC_5V_TS,             //2
    MICO_ADC_AIR_TS,            //3
    MICO_ADC_24V_ALL_C,         //4  
    MICO_ADC_12V_ALL_C,         //5
    MICO_ADC_5V_ALL_C,          //6
    MICO_ADC_24V_V,             //7
    MICO_ADC_12V_V,             //8
    MICO_ADC_5V_V,              //9
    MICO_ADC_BAT_V,             //10
    MICO_ADC_5V_SENSOR_BOARD_C, //11
    MICO_ADC_12V_ROUTER_C,       //12
    
    MICO_ADC_NV_24V_C,          //13
    MICO_ADC_NV_12V_C,          //14
    MICO_ADC_KEYPAD_C,          //15
#endif  
    /* end of vitual adc */
    MICO_ADC_MAX, /* Denotes the total number of ADC port aliases. Not a valid ADC alias */
    MICO_ADC_NONE,
} mico_adc_t;

typedef enum
{
    MICO_UART_1,
    MICO_UART_2,
    MICO_UART_3,
    MICO_UART_4,
    MICO_UART_5,
    MICO_UART_MAX, /* Denotes the total number of UART port aliases. Not a valid UART alias */
    MICO_UART_NONE,
} mico_uart_t;

typedef enum
{
  MICO_FLASH_EMBEDDED,
  MICO_FLASH_MAX,
  MICO_FLASH_NONE,
} mico_flash_t;

typedef enum
{
  MICO_PARTITION_USER_MAX
} mico_user_partition_t;

typedef enum 
{
    MICO_CAN1,
    MICO_CAN_MAX,
    MICO_CAN_NONE,
} mico_can_t;

#ifdef BOOTLOADER
#define STDIO_UART          MICO_UART_3
#define STDIO_UART_BAUDRATE (115200) 
#else




#define STDIO_UART          MICO_UART_3
#define STDIO_UART_BAUDRATE (115200) 
#endif

#define COMM_UART        MICO_UART_1
#define COMM_UART_BAUDRATE (115200) 
#define UART_FOR_APP     MICO_UART_1
#define CLI_UART         MICO_UART_3

#define BATT_UART           MICO_UART_4
#define BATT_UART_BAUDRATE  (9600)  

#ifdef NOAH_HW_V0_1
#define SERIALS_LED_UART    MICO_UART_2
#define SERIALS_LED_UART_BAUDRARE   (115200)
#endif

#ifdef __cplusplus
} /*extern "C" */
#endif

