/*
 *  Author: Adam Huang
 *  Date:2016/6/4
 */
#include "app_platform.h"
#include <stdlib.h>
#include "board_init.h"
#include "serial_leds.h"
#include "protocol.h"
#include "upgrade_flash.h"
#include "battery.h"

//#define JOY_TEST

#define platform_log(M, ...) custom_log("Platform", M, ##__VA_ARGS__)
#define platform_log_trace() custom_log_trace("Platform")

static switch_t ramSwitch_user;
switch_t *switch_user = &ramSwitch_user;

static board_status_t ramBoardStatus;
board_status_t *boardStatus = &ramBoardStatus;

static control_signal_t ramDLP_ControlSignal, ramPAD_ControlSignal, ramX86_ControlSignal;
static control_signal_t ramNV_ControlSignal;

control_signal_t *DLP_ControlSignal = &ramDLP_ControlSignal;
control_signal_t *PAD_ControlSignal = &ramPAD_ControlSignal;
control_signal_t *X86_ControlSignal = &ramX86_ControlSignal;
control_signal_t *NV_ControlSignal = &ramNV_ControlSignal;

static void switch_tick(void);
static void ctrl_signal_tick(void);
static void board_status_tick(void);

OSStatus platform_init(void)
{
    OSStatus err = kNoErr;

    require_action(switch_user, exit, err = kNoMemoryErr);
    memset(switch_user, 0x0, sizeof(switch_t));
    if(switch_user)
    {
        switch_user->switch_on_off = SWITCH_OFF;
        switch_user->debounce_time = SWITCH_DEBOUNCE_TIME;
        switch_user->get_switch_state = get_switch_state;
        switch_user->pre_io_state = switch_user->get_switch_state(SWITCH_USER);
    }

    require_action(boardStatus, exit, err = kNoMemoryErr);
    memset(boardStatus, 0x0, sizeof(board_status_t));
    if(boardStatus)
    {
        boardStatus->is_power_off_finished = YES;
        boardStatus->is_power_on_finished = YES;
        boardStatus->devices_on_off = DEVICES_OFF;
        boardStatus->set_power_on_off = power_ctrl;
        boardStatus->irled_duty = 50;
        boardStatus->bat_level = 100;
    }

    require_action(DLP_ControlSignal, exit, err = kNoMemoryErr);
    memset(DLP_ControlSignal, 0x0, sizeof(control_signal_t));
    if(DLP_ControlSignal)
    {
        DLP_ControlSignal->power_on_delay_time = DLP_POWER_ON_DELAY_TIME;
        DLP_ControlSignal->power_off_delay_time = DLP_POWER_OFF_DELAY_TIME;
        DLP_ControlSignal->power_on_hold_time = DLP_POWER_ON_HOLD_TIME;
        DLP_ControlSignal->power_off_hold_time = DLP_POWER_OFF_HOLD_TIME;
        DLP_ControlSignal->setControlSigal = module_signal_ctrl;
    }

    require_action(PAD_ControlSignal, exit, err = kNoMemoryErr);
    memset(PAD_ControlSignal, 0x0, sizeof(control_signal_t));
    if(PAD_ControlSignal)
    {
        PAD_ControlSignal->power_on_delay_time = PAD_POWER_ON_DELAY_TIME;
        PAD_ControlSignal->power_off_delay_time = PAD_POWER_OFF_DELAY_TIME;
        PAD_ControlSignal->power_on_hold_time = PAD_POWER_ON_HOLD_TIME;
        PAD_ControlSignal->power_off_hold_time = PAD_POWER_OFF_HOLD_TIME;
        PAD_ControlSignal->setControlSigal = module_signal_ctrl;
    }

    require_action(X86_ControlSignal, exit, err = kNoMemoryErr);
    memset(X86_ControlSignal, 0x0, sizeof(control_signal_t));
    if(X86_ControlSignal)
    {
        X86_ControlSignal->power_on_delay_time = X86_POWER_ON_DELAY_TIME;
        X86_ControlSignal->power_off_delay_time = X86_POWER_OFF_DELAY_TIME;
        X86_ControlSignal->power_on_hold_time = X86_POWER_ON_HOLD_TIME;
        X86_ControlSignal->power_off_hold_time = X86_POWER_OFF_HOLD_TIME;
        X86_ControlSignal->setControlSigal = module_signal_ctrl;
    }

    require_action(NV_ControlSignal, exit, err = kNoMemoryErr);
    memset(NV_ControlSignal, 0x0, sizeof(control_signal_t));
    if(NV_ControlSignal)
    {
        NV_ControlSignal->power_on_delay_time = NV_POWER_ON_DELAY_TIME;
        NV_ControlSignal->power_off_delay_time = NV_POWER_OFF_DELAY_TIME;
        NV_ControlSignal->power_on_hold_time = NV_POWER_ON_HOLD_TIME;
        NV_ControlSignal->power_off_hold_time = NV_POWER_OFF_HOLD_TMP_TIME;
        NV_ControlSignal->setControlSigal = module_signal_ctrl;
    }
exit:
    if(err != kNoErr)
    {
        platform_log("platform init error: no enough memory!");
    }
    else
    {
        platform_log("platform init success!");
    }
    return err;
}

#define HW_TEST     0
extern void serial_leds_uart_init(void);
void power_on_devices(void)
{
    if(DEVICES_OFF == boardStatus->devices_on_off)
    {
        if((YES == boardStatus->is_power_off_finished) && (YES == boardStatus->is_power_on_finished))
        {
            boardStatus->startTime = os_get_time();
            boardStatus->is_power_on_finished = NO;
#if  HW_TEST
            boardStatus->set_power_on_off(POWER_ALL , POWER_ON);
#else
            boardStatus->set_power_on_off(POWER_VSYS_24V_NV + POWER_CAMERA_BACK_LED + POWER_CAMERA_FRONT_LED +  POWER_DOOR_CTRL, POWER_OFF);
            boardStatus->set_power_on_off(POWER_ALL - (POWER_VSYS_24V_NV +  POWER_CAMERA_BACK_LED + POWER_CAMERA_FRONT_LED +  POWER_DOOR_CTRL + \
                        POWER_3V3_CARD_EN_1 + POWER_3V3_CARD_EN_2 + POWER_3V3_CARD_EN_3 + POWER_3V3_CARD_EN_4), POWER_ON);
#endif


            //boardStatus->set_power_on_off(POWER_VSYS_24V_NV, POWER_OFF);
            //boardStatus->set_power_on_off(POWER_DOOR_CTRL, POWER_OFF);
            //boardStatus->set_power_on_off(POWER_CAMERA_BACK_LED, POWER_OFF);
            //boardStatus->set_power_on_off(POWER_CAMERA_FRONT_LED, POWER_OFF);
            en_led_mcu();
            serial_leds_uart_init();


            DLP_ControlSignal->is_device_process_finished = NO;
            PAD_ControlSignal->is_device_process_finished = NO;
            X86_ControlSignal->is_device_process_finished = NO;
            NV_ControlSignal->is_device_process_finished = NO;

            DLP_ControlSignal->startTime = os_get_time();
            PAD_ControlSignal->startTime = os_get_time();
            X86_ControlSignal->startTime = os_get_time();
            NV_ControlSignal->startTime = os_get_time();

            platform_log("devices start to boot");
            boardStatus->system_status &= ~(STATE_RUN_BITS);
            boardStatus->system_status |= STATE_IS_POWER_ON;
            set_serial_leds_effect(LIGHTS_MODE_NOMAL, NULL, 0);
        }
    }
}

void power_off_devices(void)
{
    if(DEVICES_ON == boardStatus->devices_on_off)
    {
        if((YES == boardStatus->is_power_on_finished) && (YES == boardStatus->is_power_off_finished))
        {
            DLP_ControlSignal->is_device_process_finished = NO;
            PAD_ControlSignal->is_device_process_finished = NO;
            X86_ControlSignal->is_device_process_finished = NO;
            NV_ControlSignal->is_device_process_finished = NO;

            DLP_ControlSignal->startTime = os_get_time();
            PAD_ControlSignal->startTime = os_get_time();
            X86_ControlSignal->startTime = os_get_time();
            NV_ControlSignal->startTime = os_get_time();

            boardStatus->startTime = os_get_time();
            boardStatus->is_power_off_finished = NO;
            platform_log("devices start to shutdown");
            boardStatus->system_status &= ~(STATE_RUN_BITS);
            boardStatus->system_status |= STATE_IS_POWER_OFF;
            set_serial_leds_effect(LIGHTS_MODE_NOMAL, NULL, 0);
        }
    }
}

void set_module_power_signal(uint8_t module, uint8_t onoff)
{
    switch(module)
    {
        case PAD_MODULE:
            if(onoff)
            {
                if(PAD_ControlSignal->device_on_off == DEVICE_POWER_OFF)
                {
                    PAD_ControlSignal->is_device_process_finished = NO;
                    PAD_ControlSignal->startTime = os_get_time();
                }
            }
            else
            {
                if(PAD_ControlSignal->device_on_off == DEVICE_POWER_ON)
                {
                    PAD_ControlSignal->is_device_process_finished = NO;
                    PAD_ControlSignal->startTime = os_get_time();
                }
            }
            break;
        case X86_MODULE:
            if(onoff)
            {
                if(X86_ControlSignal->device_on_off == DEVICE_POWER_OFF)
                {
                    X86_ControlSignal->is_device_process_finished = NO;
                    X86_ControlSignal->startTime = os_get_time();
                }
            }
            else
            {
                if(X86_ControlSignal->device_on_off == DEVICE_POWER_ON)
                {
                    X86_ControlSignal->is_device_process_finished = NO;
                    X86_ControlSignal->startTime = os_get_time();
                }
            }
            break;
        case NV_MODULE:
            if(onoff)
            {
                if(NV_ControlSignal->device_on_off == DEVICE_POWER_OFF)
                {
                    NV_ControlSignal->is_device_process_finished = NO;
                    NV_ControlSignal->startTime = os_get_time();
                }
            }
            else
            {
                if(NV_ControlSignal->device_on_off == DEVICE_POWER_ON)
                {
                    NV_ControlSignal->is_device_process_finished = NO;
                    NV_ControlSignal->startTime = os_get_time();
                }
            }
            break;
        case DLP_MODULE:
            if(onoff)
            {
                if(DLP_ControlSignal->device_on_off == DEVICE_POWER_OFF)
                {
                    DLP_ControlSignal->is_device_process_finished = NO;
                    DLP_ControlSignal->startTime = os_get_time();
                }
            }
            else
            {
                if(DLP_ControlSignal->device_on_off == DEVICE_POWER_ON)
                {
                    DLP_ControlSignal->is_device_process_finished = NO;
                    DLP_ControlSignal->startTime = os_get_time();
                }
            }
            break;
        default:
            break;
    }
}


//#define POWER_ON_DEVICE_DELAY_TIME      1500/SYSTICK_PERIOD
//#define POWER_OFF_DEVICE_DELAY_TIME      1500/SYSTICK_PERIOD
extern void upload_power_off_signal(uint32_t second);
#define POWER_OFF_DELAY_TIME            0//10*1000/SYSTICK_PERIOD
#define GIVE_UP_TIME                    SWITCH_DEBOUNCE_TIME + 2*1000/SYSTICK_PERIOD
static void switch_tick(void)
{
#if 0

    static uint32_t switch_on_start_time = 0;
    static uint32_t switch_off_start_time = 0;
    static uint32_t power_off_delay_start_time = 0;
    static uint32_t give_up_start_time = 0;
    uint8_t switch_state = 0;

    if(STATE_POWER_OFF == (boardStatus->system_status & STATE_RUN_BITS))
    {
        switch_state = switch_user->get_switch_state(SWITCH_USER);


        if(switch_state == 0)
        {
            switch_on_start_time = os_get_time();
        }
        if(os_get_time() - switch_on_start_time >= SWITCH_DEBOUNCE_TIME)
        {
            //if((battery_pack.percentage > VBAT_POWER_OFF_PERCENTAGE + 5) || (battery_pack.com_status == false))
            {
                power_on_devices();
            }

        }

        if(os_get_time() - give_up_start_time >= GIVE_UP_TIME)
        {
            boardStatus->set_power_on_off(POWER_CTRL_OUT, POWER_OFF);
        }
    }

    if(STATE_POWER_ON == (boardStatus->system_status & STATE_RUN_BITS))
    {
        switch_state = switch_user->get_switch_state(SWITCH_USER);
        if(switch_state == 1)
        {
            switch_off_start_time = os_get_time();
        }
        if(power_off_delay_start_time == 0)
        {
            if(os_get_time() - switch_off_start_time >= SWITCH_DEBOUNCE_TIME)
            {
                power_off_delay_start_time = os_get_time();
                upload_power_off_signal(POWER_OFF_DELAY_TIME * SYSTICK_PERIOD / 1000);
            }
        }

        if(power_off_delay_start_time > 0)
        {
            if(os_get_time() - power_off_delay_start_time >= POWER_OFF_DELAY_TIME)
            {
                power_off_devices();
                power_off_delay_start_time = 0;
            }
        }
    }

#else

    //#define DEVICE_STATE_POWER_ON       1
    //#define DEVICE_STATE_POWER_OFF      0

    static uint8_t state_change = 0;

    if(boardStatus->remote_device_power_ctrl == 0)
    {
        if((boardStatus->is_power_off_finished == YES) && (boardStatus->is_power_on_finished == YES))
        {
            if(state_change == 0)
            {
                if(switch_user->pre_io_state != switch_user->get_switch_state(SWITCH_USER))
                {
                    switch_user->pre_io_state = switch_user->get_switch_state(SWITCH_USER);
                    state_change = 1;
                    platform_log("switch have switched");
                }
            }

            if(state_change == 1)
            {
                if(switch_user->pre_io_state != switch_user->get_switch_state(SWITCH_USER))
                {
                    switch_user->pre_io_state = switch_user->get_switch_state(SWITCH_USER);
                    state_change = 0;
                    switch_user->start_time = 0;
                    return ;
                }
                if(switch_user->start_time == 0)
                {
                    switch_user->start_time = os_get_time();
                }
                if((switch_user->start_time != 0) && ((os_get_time() - switch_user->start_time) >= SWITCH_DEBOUNCE_TIME))
                {
                    switch_user->start_time = 0;
#if 0
                    if(switch_user->pre_io_state != switch_user->get_switch_state(SWITCH_USER))
                    {
                        switch_user->pre_io_state = switch_user->get_switch_state(SWITCH_USER);
                    }
                    else
                    {
                        return;
                    }
#endif

                    switch_user->is_switch_finished = YES;
                    if((STATE_POWER_OFF == (boardStatus->system_status & STATE_RUN_BITS))/* && (voltageConvert->bat_voltage >= VBAT_POWER_ON_LEVEL)*/)
                    {
                        switch_user->switch_on_off = SWITCH_ON;
                        platform_log("confirm to ON stable");
                        if(boardStatus->system_status & STATE_IS_LOW_POWER)
                        {
                            boardStatus->system_status &= ~STATE_IS_LOW_POWER;
                        }
                        power_on_devices();
                        state_change = 0;
                    }

                    else if((STATE_POWER_ON == (boardStatus->system_status & STATE_RUN_BITS)))
                    {
                        switch_user->switch_on_off = SWITCH_OFF;
                        platform_log("confirm to OFF stable");

                        power_off_devices();
                        state_change = 0;
                    }
                }
            }
        }
        else
        {
            switch_user->pre_io_state = switch_user->get_switch_state(SWITCH_USER);
        }
    }
    else if(DEVICES_ON == boardStatus->devices_on_off)
    {
        //if(boardStatus->remote_device_power_ctrl == REMOTE_DEVICE_POWER_SHUTDOWN)
        {

#define REMOTE_POWER_CTRL_STATUS_DELAY      1
#define REMOTE_POWER_CTRL_STATUS_PROCEED    2
#define REMOTE_POWER_CTRL_STATUS_FINISHED   3

#define REMOTE_POWER_CTRL_DELAY_TIME        5000/SYSTICK_PERIOD

            static uint8_t status = 0;
            static uint32_t remote_power_delay_start_time = 0;

            switch (status)
            {
                case 0:
                    status = REMOTE_POWER_CTRL_STATUS_DELAY;
                    remote_power_delay_start_time = os_get_time();
                    break;
                case REMOTE_POWER_CTRL_STATUS_DELAY:
                    if(os_get_time() - remote_power_delay_start_time > REMOTE_POWER_CTRL_DELAY_TIME)
                    {
                        status = REMOTE_POWER_CTRL_STATUS_PROCEED;
                    }
                    break;
                case REMOTE_POWER_CTRL_STATUS_PROCEED:
                    if(boardStatus->remote_device_power_ctrl == REMOTE_DEVICE_POWER_SHUTDOWN)
                    {
                        power_off_devices();
                        status = REMOTE_POWER_CTRL_STATUS_FINISHED;
                    }
                    else if(boardStatus->remote_device_power_ctrl == REMOTE_DEVICE_POWER_REBOOT)
                    {
                        power_off_devices();
                        boardStatus->reboot_flag  = REBOOT_YES;
                    }

                    break;
                case REMOTE_POWER_CTRL_STATUS_FINISHED:
                    //status = 0;
                    break;

                default:
                    break;
            }
        }
        //else if(boardStatus->remote_device_power_ctrl == REMOTE_DEVICE_POWER_REBOOT)
        {

        }



    }

#endif
}

static void ctrl_signal_tick(void)
{

    /* pad */
    if((NO == PAD_ControlSignal->is_device_process_finished) && (PAD_ControlSignal->setControlSigal != NULL))
    {
        if((DEVICE_POWER_OFF == PAD_ControlSignal->device_on_off) && (PAD_ControlSignal->startTime != 0))
        {
            switch(PAD_ControlSignal->period)
            {
                case SIGNAL_DELAY:
                    if(os_get_time() - PAD_ControlSignal->startTime >= PAD_POWER_ON_DELAY_TIME)
                    {
                        PAD_ControlSignal->startTime = os_get_time();
                        PAD_ControlSignal->setControlSigal(POWER_CON_PAD, CONTROL_HOLD);
                        PAD_ControlSignal->period = SIGNAL_HOLD;
                        platform_log("pad boot signal hold on");
                    }
                    break;
                case SIGNAL_HOLD:
                    if(os_get_time() - PAD_ControlSignal->startTime >= PAD_POWER_ON_HOLD_TIME)
                    {
                        PAD_ControlSignal->startTime = os_get_time();
                        PAD_ControlSignal->setControlSigal(POWER_CON_PAD, CONTROL_RELEASE);
                        PAD_ControlSignal->period = SIGNAL_REALSE;
                        platform_log("pad boot signal hold off");
                    }
                    break;
                case SIGNAL_REALSE:
                    if(os_get_time() - PAD_ControlSignal->startTime >= PAD_POWER_ON_PROCESSING_TIME)
                    {
                        PAD_ControlSignal->startTime = 0;
                        PAD_ControlSignal->period = SIGNAL_DELAY;
                        PAD_ControlSignal->device_on_off = DEVICE_POWER_ON;
                        PAD_ControlSignal->is_device_process_finished = YES;
                        platform_log("pad boot have powered on");
                    }
                    break;
                default:
                    break;
            }
        }
        else if((DEVICE_POWER_ON == PAD_ControlSignal->device_on_off) && (PAD_ControlSignal->startTime != 0))
        {
            switch(PAD_ControlSignal->period)
            {
                case SIGNAL_DELAY:
                    if(os_get_time() - PAD_ControlSignal->startTime >= PAD_POWER_OFF_DELAY_TIME)
                    {
                        PAD_ControlSignal->startTime = os_get_time();
                        PAD_ControlSignal->setControlSigal(POWER_CON_PAD, CONTROL_HOLD);
                        PAD_ControlSignal->period = SIGNAL_HOLD;
                        platform_log("pad shutdown signal hold on");
                    }
                    break;
                case SIGNAL_HOLD:
                    if(os_get_time() - PAD_ControlSignal->startTime >= PAD_POWER_OFF_HOLD_TIME)
                    {
                        PAD_ControlSignal->startTime = os_get_time();
                        PAD_ControlSignal->setControlSigal(POWER_CON_PAD, CONTROL_RELEASE);
                        PAD_ControlSignal->period = SIGNAL_REALSE;
                        platform_log("pad shutdown signal hold off");
                    }
                    break;
                case SIGNAL_REALSE:
                    if(os_get_time() - PAD_ControlSignal->startTime >= PAD_POWER_OFF_PROCESSING_TIME)
                    {
                        PAD_ControlSignal->startTime = 0;
                        PAD_ControlSignal->period = SIGNAL_DELAY;
                        PAD_ControlSignal->device_on_off = DEVICE_POWER_OFF;
                        PAD_ControlSignal->is_device_process_finished = YES;
                        platform_log("pad shutdown have powered off");
                    }
                    break;
                default:
                    break;
            }
        }
    }
#if 1
    /*  nv */
    if((DEVICE_POWER_OFF == NV_ControlSignal->device_on_off) && (NV_ControlSignal->startTime != 0))
    {
        switch(NV_ControlSignal->period)
        {
            case SIGNAL_DELAY:
                if(os_get_time() - NV_ControlSignal->startTime >= NV_POWER_ON_DELAY_TIME)
                {
                    NV_ControlSignal->startTime = os_get_time();
                    NV_ControlSignal->setControlSigal(POWER_CON_NV, CONTROL_HOLD);
                    NV_ControlSignal->setControlSigal(POWER_CON_RES, CONTROL_HOLD);//joy test
                    NV_ControlSignal->period = SIGNAL_HOLD;
                    platform_log("NV boot signal hold on");
                }
                break;

            case SIGNAL_HOLD:
                if(os_get_time() - NV_ControlSignal->startTime >= NV_POWER_ON_HOLD_TIME)
                {
                    NV_ControlSignal->startTime = os_get_time();
                    NV_ControlSignal->setControlSigal(POWER_CON_NV, CONTROL_RELEASE);
                    NV_ControlSignal->setControlSigal(POWER_CON_RES, CONTROL_RELEASE);//joy test
                    NV_ControlSignal->period = SIGNAL_REALSE;
                    platform_log("NV boot signal hold off");
                }
                break;

            case SIGNAL_REALSE:
                if(os_get_time() - NV_ControlSignal->startTime >= NV_POWER_ON_PROCESSING_TMP_TIME)
                {
                    NV_ControlSignal->startTime = 0;
                    NV_ControlSignal->period = SIGNAL_DELAY;
                    NV_ControlSignal->device_on_off = DEVICE_POWER_ON;
                    NV_ControlSignal->is_device_process_finished = YES;
                    platform_log("NV boot have powered on");
                }
                break;
            default:
                break;
        }
    }
    else if((DEVICE_POWER_ON == NV_ControlSignal->device_on_off) && (NV_ControlSignal->startTime != 0))
    {
        switch(NV_ControlSignal->period)
        {
            case SIGNAL_DELAY:
                if(os_get_time() - NV_ControlSignal->startTime >= NV_POWER_OFF_DELAY_TIME)
                {
                    NV_ControlSignal->startTime = os_get_time();
                    NV_ControlSignal->setControlSigal(POWER_CON_NV, CONTROL_HOLD);
                    NV_ControlSignal->setControlSigal(POWER_CON_RES, CONTROL_HOLD);//joy test
                    NV_ControlSignal->period = SIGNAL_HOLD;
                    platform_log("NV shutdown signal hold on");
                }
                break;
            case SIGNAL_HOLD:
                if(os_get_time() - NV_ControlSignal->startTime >= NV_POWER_OFF_HOLD_TMP_TIME)
                {
                    NV_ControlSignal->startTime = os_get_time();
                    NV_ControlSignal->setControlSigal(POWER_CON_NV, CONTROL_RELEASE);
                    NV_ControlSignal->setControlSigal(POWER_CON_RES, CONTROL_RELEASE);//joy test
                    NV_ControlSignal->period = SIGNAL_REALSE;
                    platform_log("NV shutdown signal hold off");
                }
                break;
            case SIGNAL_REALSE:
                if(os_get_time() - NV_ControlSignal->startTime >= NV_POWER_OFF_PROCESSING_TIME)
                {
                    NV_ControlSignal->startTime = 0;
                    NV_ControlSignal->period = SIGNAL_DELAY;
                    NV_ControlSignal->device_on_off = DEVICE_POWER_OFF;
                    NV_ControlSignal->is_device_process_finished = YES;
                    platform_log("NV shutdown have powered off");
                }
                break;
            default:
                break;
        }
    }
    /* x86 */
    if((NO == X86_ControlSignal->is_device_process_finished) && (X86_ControlSignal->setControlSigal != NULL))
    {
        if((DEVICE_POWER_OFF == X86_ControlSignal->device_on_off) && (X86_ControlSignal->startTime != 0))
        {
            switch(X86_ControlSignal->period)
            {
                case SIGNAL_DELAY:
                    if(os_get_time() - X86_ControlSignal->startTime >= X86_POWER_ON_DELAY_TIME)
                    {
                        X86_ControlSignal->startTime = os_get_time();
                        X86_ControlSignal->setControlSigal(POWER_CON_X86, CONTROL_HOLD);
                        X86_ControlSignal->period = SIGNAL_HOLD;
                        platform_log("x86 boot signal hold on");
                    }
                    break;
                case SIGNAL_HOLD:
                    if(os_get_time() - X86_ControlSignal->startTime >= X86_POWER_ON_HOLD_TIME)
                    {
                        X86_ControlSignal->startTime = os_get_time();
                        X86_ControlSignal->setControlSigal(POWER_CON_X86, CONTROL_RELEASE);
                        X86_ControlSignal->period = SIGNAL_REALSE;
                        platform_log("x86 boot signal hold off");
                    }
                    break;
                case SIGNAL_REALSE:
                    if(os_get_time() - X86_ControlSignal->startTime >= X86_POWER_ON_PROCESSING_TIME)
                    {
                        X86_ControlSignal->startTime = 0;
                        X86_ControlSignal->period = SIGNAL_DELAY;
                        X86_ControlSignal->device_on_off = DEVICE_POWER_ON;
                        X86_ControlSignal->is_device_process_finished = YES;
                        platform_log("x86 boot have powered on");
                    }
                    break;
                default:
                    break;
            }
        }
        else if((DEVICE_POWER_ON == X86_ControlSignal->device_on_off) && (X86_ControlSignal->startTime != 0))
        {
            switch(X86_ControlSignal->period)
            {
                case SIGNAL_DELAY:
                    if(os_get_time() - X86_ControlSignal->startTime >= X86_POWER_OFF_DELAY_TIME)
                    {
                        X86_ControlSignal->startTime = os_get_time();
                        X86_ControlSignal->setControlSigal(POWER_CON_X86, CONTROL_HOLD);
                        X86_ControlSignal->period = SIGNAL_HOLD;
                        platform_log("x86 shutdown signal hold on");
                    }
                    break;
                case SIGNAL_HOLD:
                    if(os_get_time() - X86_ControlSignal->startTime >= X86_POWER_OFF_HOLD_TIME)
                    {
                        X86_ControlSignal->startTime = os_get_time();
                        X86_ControlSignal->setControlSigal(POWER_CON_X86, CONTROL_RELEASE);
                        X86_ControlSignal->period = SIGNAL_REALSE;
                        platform_log("x86 shutdown signal hold off");
                    }
                    break;
                case SIGNAL_REALSE:
                    if(os_get_time() - X86_ControlSignal->startTime >= X86_POWER_OFF_PROCESSING_TIME)
                    {
                        X86_ControlSignal->startTime = 0;
                        X86_ControlSignal->period = SIGNAL_DELAY;
                        X86_ControlSignal->device_on_off = DEVICE_POWER_OFF;
                        X86_ControlSignal->is_device_process_finished = YES;
                        platform_log("x86 shutdown have powered off");
                    }
                    break;
                default:
                    break;
            }
        }
    }
#endif
}

static void board_status_tick(void)
{
    // boot
    if((NO == boardStatus->is_power_on_finished) && (YES == boardStatus->is_power_off_finished))
    {
        if(os_get_time() - boardStatus->startTime >= POWER_ON_PROCEESING_TIME)
        {
            boardStatus->startTime = 0;
            boardStatus->is_power_on_finished = YES;
            boardStatus->devices_on_off = DEVICES_ON;
            platform_log("all devices boot finished!");
            boardStatus->system_status &= ~(STATE_RUN_BITS);
            boardStatus->system_status |= STATE_POWER_ON;

            if(serial->rx_info->start_time == 0)
            {
                //set_serial_leds_effect(LIGHTS_MODE_ERROR, NULL, 0);
            }
            else
            {
                set_serial_leds_effect(LIGHTS_MODE_NOMAL, NULL, 0);
            }

#ifdef JOY_TEST
            boardStatus->reboot_flag = 1;
            power_off_devices();
#endif
        }
    }
    //shutdown
    if((NO == boardStatus->is_power_off_finished) && (YES == boardStatus->is_power_on_finished))
    {
        if(os_get_time() - boardStatus->startTime >= POWER_OFF_PROCEESING_TIME)
        {
            boardStatus->startTime = 0;
            boardStatus->is_power_off_finished = YES;
            boardStatus->set_power_on_off(POWER_ALL, POWER_OFF);
            boardStatus->devices_on_off = DEVICES_OFF;
            platform_log("all devices shutdown finished");
            boardStatus->system_status &= ~(STATE_RUN_BITS);
            boardStatus->system_status |= STATE_POWER_OFF;
            if((REBOOT_YES == boardStatus->reboot_flag)
                    && (voltageConvert->bat_voltage >= VBAT_POWER_ON_LEVEL))
            {
                boardStatus->startTime = os_get_time();
                //platform_log("board start to reboot after 5 seconds");
                platform_log("board start to reboot right now");
                flashTable.is_need_auto_reboot = 'Y';
                flashTable.auto_boot_delay_time = 10;
                MICOBootConfiguration(&flashTable);
                NVIC_SystemReset();
            }
            else
            {
                NVIC_SystemReset();
                boardStatus->system_status |= SYSTEM_IS_SLEEP;
                enter_sleep_mode();
            }
        }
    }
    if((REBOOT_YES == boardStatus->reboot_flag) && (YES == boardStatus->is_power_off_finished))
    {
        if(os_get_time() - boardStatus->startTime >= BOARD_REBOOT_WAITING_TIME)
        {
            power_on_devices();
            boardStatus->reboot_flag = REBOOT_NO;
        }
    }
}

#define SWITCH_STATE_ON         ((uint32_t)0x00000001)
#define DLP_STATE_ON            ((uint32_t)0x00000002)
#define X86_STATE_ON            ((uint32_t)0x00000004)
#define PAD_STATE_ON            ((uint32_t)0x00000008)
#define DC_5V_STATE_ON          ((uint32_t)0x00000010)
#define DC_12V_STATE_ON         ((uint32_t)0x00000020)
#define DC_24V_STATE_ON         ((uint32_t)0x00000040)
#define MOTOR_STATE_ON          ((uint32_t)0x00000080)
#define SENSOR_STATE_ON         ((uint32_t)0x00000100)
#define LEDS_STATE_ON           ((uint32_t)0x00000200)
#define RES_5V_STATE_ON         ((uint32_t)0x00000400)
#define ROUTER_STATE_ON         ((uint32_t)0x00000800)
#define PA_2_1_STATE_ON         ((uint32_t)0x00001000)
#define NV_STATE_ON             ((uint32_t)0x00002000)
#define PRINTER_STATE_ON        ((uint32_t)0x00004000)
#define DYP_STATE_ON            ((uint32_t)0x00008000)
#define RES_12V_STATE_ON        ((uint32_t)0x00010000)
#define RES_24V_STATE_ON        ((uint32_t)0x00020000)
#define BAT_NV_STATE_ON         ((uint32_t)0x00040000)
#define AIUI_STATE_ON           ((uint32_t)0x00080000)


uint32_t get_modules_states(void)
{
    uint32_t states;

    states = get_module_power_state(POWER_ALL);
#if 0
    if(SWITCH_ON == switch_user->switch_on_off)
    {
        states |= SWITCH_STATE_ON;
    }
    //if(DEVICE_POWER_ON == DLP_ControlSignal->device_on_off)
    {
        //states |= POWER_DLP;
    }
    if(DEVICE_POWER_ON == X86_ControlSignal->device_on_off)
    {
        states |= POWER_X86;
    }
    if(DEVICE_POWER_ON == PAD_ControlSignal->device_on_off)
    {
        states |= POWER_PAD;
    }
    if(DEVICE_POWER_ON == NV_ControlSignal->device_on_off)
    {
        states |= POWER_NV;
    }
#endif
    return states;
}

extern void upload_sys_state(void);
void charge_plug_in_callback(void)
{
    if(MicoGpioInputGet(MICO_GPIO_CHARGE_IN) == 1)
    {
        boardStatus->system_status |= STATE_IS_CHARGER_IN;
    }
    else
    {
        boardStatus->system_status &= ~STATE_IS_CHARGER_IN;
    }
    if(MicoGpioInputGet(MICO_GPIO_RECHARGE_IN) == 1)
    {
        boardStatus->system_status |= STATE_IS_RECHARGE_IN;
    }
    else
    {
        boardStatus->system_status &= ~STATE_IS_RECHARGE_IN;
    }

    if((MicoGpioInputGet(MICO_GPIO_CHARGE_IN) == 0) && (MicoGpioInputGet(MICO_GPIO_RECHARGE_IN) == 0))
    {
        boardStatus->system_status &= ~STATE_IS_CHARGER_IN;
        boardStatus->system_status &= ~STATE_IS_RECHARGE_IN;
        boardStatus->system_status &= ~STATE_IS_CHARGING;
    }
    else
    {
        if(battery_pack.pack_voltage < 100)
        {
            boardStatus->system_status |= STATE_IS_CHARGING;
        }
    }
    //AckReadSysStatusFrameProcess(serial, 0);

    upload_sys_state();

    MicoGpioOutputHigh((mico_gpio_t)MICO_GPIO_CHARGE_FAN_CTRL);//charger plug in fan ctrl
}

void charge_unplug_callback(void)
{
    if(MicoGpioInputGet(MICO_GPIO_CHARGE_IN) == 1)
    {
        boardStatus->system_status |= STATE_IS_CHARGER_IN;
    }
    else
    {
        boardStatus->system_status &= ~STATE_IS_CHARGER_IN;
    }
    if(MicoGpioInputGet(MICO_GPIO_RECHARGE_IN) == 1)
    {
        boardStatus->system_status |= STATE_IS_RECHARGE_IN;
    }
    else
    {
        boardStatus->system_status &= ~STATE_IS_RECHARGE_IN;
    }

    if((MicoGpioInputGet(MICO_GPIO_CHARGE_IN) == 0) && (MicoGpioInputGet(MICO_GPIO_RECHARGE_IN) == 0))
    {
        boardStatus->system_status &= ~STATE_IS_CHARGER_IN;
        boardStatus->system_status &= ~STATE_IS_RECHARGE_IN;
        boardStatus->system_status &= ~STATE_IS_CHARGING;
    }
    else
    {
        if(battery_pack.pack_voltage < 100)
        {
            boardStatus->system_status |= STATE_IS_CHARGING;
        }
    }
    MicoGpioOutputLow((mico_gpio_t)MICO_GPIO_CHARGE_FAN_CTRL);//charger unplug fan ctrl
    upload_sys_state();
}

#define CHARGING_DEBAUNCE_TIME  20/SYSTICK_PERIOD
void detect_charge_tick(void)
{
    extern const platform_gpio_t            platform_gpio_pins[];
    static uint8_t state = 0;
    static uint32_t plug_in_start_time = 0;

    switch(state)
    {
        case 0:
            if((MicoGpioInputGet(MICO_GPIO_CHARGE_IN) == 1) || (MicoGpioInputGet(MICO_GPIO_RECHARGE_IN) == 1))
            {
                plug_in_start_time = os_get_time();
                state = 1;
            }
            break;
        case 1:
            if(os_get_time() - plug_in_start_time > CHARGING_DEBAUNCE_TIME)
            {
                state = 2;

                platform_log("charge port plug in\r\n");
                charge_plug_in_callback();

            }
            break;
        case 2:
            if((MicoGpioInputGet(MICO_GPIO_CHARGE_IN) == 0) && (MicoGpioInputGet(MICO_GPIO_RECHARGE_IN) == 0))
            {
                state = 0;
                platform_log("charge port not plug in\r\n");
                charge_unplug_callback();

            }
        default :
            break;
    }

    if((MicoGpioInputGet(MICO_GPIO_CHARGE_IN) == 0) && (MicoGpioInputGet(MICO_GPIO_RECHARGE_IN) == 0))
    {
        state = 0;
        boardStatus->system_status &= ~STATE_IS_CHARGER_IN;
        boardStatus->system_status &= ~STATE_IS_RECHARGE_IN;
        boardStatus->system_status &= ~STATE_IS_CHARGING;
    }
    else
    {
#if 0
        if(battery_pack.percentage < 100)
        {
            boardStatus->system_status |= STATE_IS_CHARGING;
        }
        else if(battery_pack.percentage == 100)
        {
            boardStatus->system_status &= ~STATE_IS_CHARGING;
        }
#endif
        if(battery_pack.pack_current > 0)
        {
            boardStatus->system_status |= STATE_IS_CHARGING;
        }
        else
        {
            boardStatus->system_status &= ~STATE_IS_CHARGING;
        }

    }

}

void platform_tick(void)
{
    switch_tick();
    ctrl_signal_tick();
    board_status_tick();
    detect_charge_tick();
}

void get_hw_version(void)
{
#define DEBOUNCE_TIME                       100/SYSTICK_PERIOD
#define GET_HARD_VERSION_OVER_TIME          2000/SYSTICK_PERIOD
    uint8_t new_key_value = 0;
    uint8_t old_key_value = 0;


    static uint32_t start_time = 0;
    start_time = os_get_time();
    uint32_t overtime_start_time = os_get_time();
    while(os_get_time() - start_time <= DEBOUNCE_TIME)
    {
        old_key_value = new_key_value;

        new_key_value |=  MicoGpioInputGet(MICO_GPIO_HW_VERSION_ID_0)^1;
        new_key_value |=  (MicoGpioInputGet(MICO_GPIO_HW_VERSION_ID_1)^1)<<1;

        if(new_key_value != old_key_value)
        {
            start_time = os_get_time();
        }
        if(os_get_time() - overtime_start_time >= GET_HARD_VERSION_OVER_TIME)
        {
            break;
        }
    }
    boardStatus->hw_version_id =  new_key_value;
    switch(boardStatus->hw_version_id)
    {
        case 0:
            memcpy(boardStatus->hw_version, HW_VERSION_V0_2, strlen(HW_VERSION_V0_2));
            break;
        case 1:
            memcpy(boardStatus->hw_version, HW_VERSION_V0_3, strlen(HW_VERSION_V0_3));
            break;
        case 2:
            break;
        case 3:
            break;
        default :
            break;
    }
}

void bsp_init(void)
{
    board_gpios_init();
    charger_detect_init();
    //key_switch_init();
}

void key_switch_interrupt_cb(void)
{
    if(boardStatus->system_status & SYSTEM_IS_SLEEP)
    {
        boardStatus->system_status &= ~(SYSTEM_IS_SLEEP);
        wake_up_from_sleep_mode();
    }
    switch_user->is_switch_finished = NO;
    switch_user->start_time = os_get_time();
    platform_log("switch have switched");
}
/*********************END OF FILE**************/
