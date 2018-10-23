/*
 *  Author: Adam Huang
 *  Date:2016/6/4
 */
#ifndef __APP_PLATFORM_H
#define __APP_PLATFORM_H

#include <stdint.h>
#include "stm32f1xx_powerboard.h"
#include "Common.h"
#include "Debug.h"

#define OWN_PAD

#define os_get_time                   HAL_GetTick

#define SYSTICK_PERIOD                10      //1ms

#define SWITCH_DEBOUNCE_TIME          1000/SYSTICK_PERIOD   //1s


#define DLP_POWER_ON_DELAY_TIME       3000/SYSTICK_PERIOD   //2s
#define DLP_POWER_OFF_DELAY_TIME      1000/SYSTICK_PERIOD   //1s
#define DLP_POWER_ON_HOLD_TIME        5000/SYSTICK_PERIOD   //5s
#define DLP_POWER_OFF_HOLD_TIME       5000/SYSTICK_PERIOD   //5s
#define DLP_POWER_ON_PROCESSING_TIME  30000/SYSTICK_PERIOD   //30s
#define DLP_POWER_OFF_PROCESSING_TIME 20000/SYSTICK_PERIOD   //20s



#define PAD_POWER_ON_DELAY_TIME       3200/SYSTICK_PERIOD   //2s
#define PAD_POWER_OFF_DELAY_TIME      1000/SYSTICK_PERIOD   //1s
#define PAD_POWER_ON_HOLD_TIME        4000/SYSTICK_PERIOD   //4s
#define PAD_POWER_OFF_HOLD_TIME       5000/SYSTICK_PERIOD   //5s
#define PAD_POWER_ON_PROCESSING_TIME  30000/SYSTICK_PERIOD   //30s
#define PAD_POWER_OFF_PROCESSING_TIME 20000/SYSTICK_PERIOD   //20s


#define X86_POWER_ON_DELAY_TIME       3500/SYSTICK_PERIOD   //1s
#define X86_POWER_OFF_DELAY_TIME      1000/SYSTICK_PERIOD   //1s
#define X86_POWER_ON_HOLD_TIME        2000/SYSTICK_PERIOD   //2s
#define X86_POWER_OFF_HOLD_TIME       500/SYSTICK_PERIOD   //0.5s
#define X86_POWER_ON_PROCESSING_TIME  15000/SYSTICK_PERIOD   //15s
#define X86_POWER_OFF_PROCESSING_TIME 15000/SYSTICK_PERIOD   //15s

#define NV_POWER_ON_DELAY_TIME                 4000/SYSTICK_PERIOD   //5s
#define NV_POWER_OFF_DELAY_TIME                1000/SYSTICK_PERIOD   //1s
#define NV_POWER_ON_HOLD_TIME                   70/SYSTICK_PERIOD   //50ms
#define NV_POWER_ON_PROCESSING_TMP_TIME         40000/SYSTICK_PERIOD   //40s
#define NV_POWER_OFF_HOLD_TMP_TIME              7500/SYSTICK_PERIOD   //7.5s
#define NV_POWER_OFF_PROCESSING_TIME            15000/SYSTICK_PERIOD   //15s

#define BOARD_REBOOT_WAITING_TIME               5000/SYSTICK_PERIOD  //5s

#define YES   1
#define NO    (!YES)

#define POWER_ON_PROCEESING_TIME      45000/SYSTICK_PERIOD   //45s
#define POWER_OFF_PROCEESING_TIME     30000/SYSTICK_PERIOD   //30s

typedef  uint32_t (*get_switch_state_fn)(switch_type_def Switch);
typedef  void (*set_power_on_off_fn)(uint32_t power_en, power_on_off_type_def on_off);
typedef  void (*set_control_signal_fn)(power_control_type_def PowerCon, control_signal_type_def is_hold);

typedef struct _switch_t{
    uint8_t       is_switch_finished;
    uint8_t       switch_on_off;
#define                 SWITCH_OFF    0x01
#define                 SWITCH_ON     0x02
    uint8_t       pre_io_state;
    uint32_t      debounce_time;
    uint32_t      start_time;
    get_switch_state_fn  get_switch_state;
} switch_t;

typedef struct _board_status_t{
    uint8_t            is_power_on_finished;
    uint8_t            is_power_off_finished;
    uint16_t           system_status;
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
    uint8_t            devices_on_off;
#define                 DEVICES_OFF    0x00
#define                 DEVICES_ON     0x01
    set_power_on_off_fn   set_power_on_off;
    uint16_t           bat_level;
    uint16_t           charger_level;
    uint8_t            reboot_flag;
#define                 REBOOT_NO      0x00
#define                 REBOOT_YES     0x01
    uint8_t            err_channel;
    uint16_t           err_value;
    uint32_t           startTime;
#ifdef MIKE_TEST
    uint32_t           charger_times;
#endif
    uint8_t            irled_duty;
    uint8_t            is_upgrading;
    uint8_t            remote_device_power_ctrl;
#define                 REMOTE_DEVICE_POWER_SHUTDOWN    1
#define                 REMOTE_DEVICE_POWER_REBOOT      2

    uint8_t            hw_version_id;
#define                 HW_V0_3     1
    char hw_version[20];

} board_status_t;

typedef struct _control_signal_t {
    uint8_t      period;
    uint8_t      is_device_process_finished;
    uint8_t      device_on_off;
#define                 DEVICE_POWER_OFF   0x00
#define                 DEVICE_POWER_ON    0x01
    uint16_t     power_on_delay_time;
    uint16_t     power_off_delay_time;
    uint16_t     power_on_hold_time;
    uint16_t     power_off_hold_time;
    set_control_signal_fn setControlSigal;
    uint32_t     startTime;
#define                SIGNAL_DELAY       0x00
#define                SIGNAL_HOLD        0x01
#define                SIGNAL_REALSE      0x02
#define                SIGNAL_HOLD_TMP1   0x03
#define                SIGNAL_REALSE_TMP  0x04
#define                SIGNAL_HOLD_TMP2   0x05
} control_signal_t;

extern switch_t *switch_user;
extern board_status_t *boardStatus;

extern void power_off_devices( void );
extern void power_on_devices( void );
extern OSStatus platform_init( void );
extern uint32_t get_modules_states( void );
extern void set_module_power_signal( uint8_t module, uint8_t onoff );
extern void platform_tick( void );
extern void bsp_init( void );
extern void config_sys_clock( void );
extern void key_switch_interrupt_cb( void );
extern void get_hw_version(void);
#endif  /* __PLATFORM_H */

