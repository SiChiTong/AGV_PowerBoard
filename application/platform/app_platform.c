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

static boardStatus_t ramBoardStatus;
boardStatus_t *boardStatus = &ramBoardStatus;

static controlSignal_t ramDLP_ControlSignal, ramPAD_ControlSignal, ramX86_ControlSignal;
static controlSignal_t ramNV_ControlSignal;

controlSignal_t *DLP_ControlSignal = &ramDLP_ControlSignal;
controlSignal_t *PAD_ControlSignal = &ramPAD_ControlSignal;
controlSignal_t *X86_ControlSignal = &ramX86_ControlSignal;
controlSignal_t *NV_ControlSignal = &ramNV_ControlSignal;

static void Switch_Tick( void );
static void ControlSignal_Tick( void );
static void BoardStatus_Tick( void );

OSStatus Platform_Init( void )
{ 
  OSStatus err = kNoErr;
  
  require_action( switch_user, exit, err = kNoMemoryErr );
  memset(switch_user, 0x0, sizeof(switch_t));
  if( switch_user )
  {
    switch_user->switchOnOff = SWITCH_OFF;
    switch_user->debounceTime = SWITCH_DEBOUNCE_TIME;
    switch_user->getSwitchState = BSP_SW_GetState;
    switch_user->preIOState = switch_user->getSwitchState( SWITCH_USER );
  }

  require_action( boardStatus, exit, err = kNoMemoryErr );
  memset(boardStatus, 0x0, sizeof(boardStatus_t));
  if( boardStatus )
  {
    boardStatus->isPowerOffFinish = YES;
    boardStatus->isPowerOnFinish = YES;
    boardStatus->devicesOnOff = DEVICES_OFF;
    boardStatus->setPowerOnoff = BSP_Power_OnOff;
    boardStatus->irled_duty = 50;
    boardStatus->vBatLevel = 100;
  }

  require_action( DLP_ControlSignal, exit, err = kNoMemoryErr );
  memset(DLP_ControlSignal, 0x0, sizeof(controlSignal_t));
  if( DLP_ControlSignal )
  {
    DLP_ControlSignal->powerOnDelayTime = DLP_POWER_ON_DELAY_TIME;
    DLP_ControlSignal->powerOffDelayTime = DLP_POWER_OFF_DELAY_TIME;
    DLP_ControlSignal->powerOnHoldTime = DLP_POWER_ON_HOLD_TIME;
    DLP_ControlSignal->powerOffHoldTime = DLP_POWER_OFF_HOLD_TIME;
    DLP_ControlSignal->setControlSigal = BSP_Control_Sigal;
  }

  require_action( PAD_ControlSignal, exit, err = kNoMemoryErr );
  memset(PAD_ControlSignal, 0x0, sizeof(controlSignal_t));
  if( PAD_ControlSignal )
  {
    PAD_ControlSignal->powerOnDelayTime = PAD_POWER_ON_DELAY_TIME;
    PAD_ControlSignal->powerOffDelayTime = PAD_POWER_OFF_DELAY_TIME;
    PAD_ControlSignal->powerOnHoldTime = PAD_POWER_ON_HOLD_TIME;
    PAD_ControlSignal->powerOffHoldTime = PAD_POWER_OFF_HOLD_TIME;
    PAD_ControlSignal->setControlSigal = BSP_Control_Sigal;
  }

  require_action( X86_ControlSignal, exit, err = kNoMemoryErr );
  memset(X86_ControlSignal, 0x0, sizeof(controlSignal_t));
  if( X86_ControlSignal )
  {
    X86_ControlSignal->powerOnDelayTime = X86_POWER_ON_DELAY_TIME;
    X86_ControlSignal->powerOffDelayTime = X86_POWER_OFF_DELAY_TIME;
    X86_ControlSignal->powerOnHoldTime = X86_POWER_ON_HOLD_TIME;
    X86_ControlSignal->powerOffHoldTime = X86_POWER_OFF_HOLD_TIME;
    X86_ControlSignal->setControlSigal = BSP_Control_Sigal;
  }
  
  require_action( NV_ControlSignal, exit, err = kNoMemoryErr );
  memset(NV_ControlSignal, 0x0, sizeof(controlSignal_t));
  if( NV_ControlSignal )
  {
    NV_ControlSignal->powerOnDelayTime = NV_POWER_ON_DELAY_TIME;
    NV_ControlSignal->powerOffDelayTime = NV_POWER_OFF_DELAY_TIME;
    NV_ControlSignal->powerOnHoldTime = NV_POWER_ON_HOLD_TIME;
    NV_ControlSignal->powerOffHoldTime = NV_POWER_OFF_HOLD_TMP_TIME;
    NV_ControlSignal->setControlSigal = BSP_Control_Sigal;
  }
exit:
  if( err != kNoErr )
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
extern void led_uart_init(void);
void PowerOnDevices( void )
{
  if( DEVICES_OFF == boardStatus->devicesOnOff )
  {
    if( (YES == boardStatus->isPowerOffFinish) && (YES == boardStatus->isPowerOnFinish) )
    {
      boardStatus->startTime = os_get_time();
      boardStatus->isPowerOnFinish = NO;
#if  HW_TEST
       boardStatus->setPowerOnoff(POWER_ALL , POWER_ON);
#else   
      boardStatus->setPowerOnoff(POWER_VSYS_24V_NV + POWER_CAMERA_BACK_LED + POWER_CAMERA_FRONT_LED +  POWER_DOOR_CTRL, POWER_OFF);
      boardStatus->setPowerOnoff(POWER_ALL - (POWER_VSYS_24V_NV +  POWER_CAMERA_BACK_LED + POWER_CAMERA_FRONT_LED +  POWER_DOOR_CTRL + \
       POWER_3V3_CARD_EN_1 + POWER_3V3_CARD_EN_2 + POWER_3V3_CARD_EN_3 + POWER_3V3_CARD_EN_4 ), POWER_ON);
#endif
      
      
      //boardStatus->setPowerOnoff(POWER_VSYS_24V_NV, POWER_OFF);
      //boardStatus->setPowerOnoff(POWER_DOOR_CTRL, POWER_OFF);
      //boardStatus->setPowerOnoff(POWER_CAMERA_BACK_LED, POWER_OFF);
      //boardStatus->setPowerOnoff(POWER_CAMERA_FRONT_LED, POWER_OFF);
      en_led_mcu();
      led_uart_init();
      

      DLP_ControlSignal->isDeviceProcessOver = NO;
      PAD_ControlSignal->isDeviceProcessOver = NO;
      X86_ControlSignal->isDeviceProcessOver = NO;
      NV_ControlSignal->isDeviceProcessOver = NO;
      
      DLP_ControlSignal->startTime = os_get_time();
      PAD_ControlSignal->startTime = os_get_time();
      X86_ControlSignal->startTime = os_get_time(); 
      NV_ControlSignal->startTime = os_get_time(); 
      
      platform_log("devices start to boot");
      boardStatus->sysStatus &= ~(STATE_RUN_BITS);
      boardStatus->sysStatus |= STATE_IS_POWER_ON;
      SetSerialLedsEffect( LIGHTS_MODE_NOMAL, NULL, 0 );
    }
  }
}

void PowerOffDevices( void )
{
  if( DEVICES_ON == boardStatus->devicesOnOff )
  {
    if( (YES == boardStatus->isPowerOnFinish) && (YES == boardStatus->isPowerOffFinish) )
    {
      DLP_ControlSignal->isDeviceProcessOver = NO;
      PAD_ControlSignal->isDeviceProcessOver = NO;
      X86_ControlSignal->isDeviceProcessOver = NO;
      NV_ControlSignal->isDeviceProcessOver = NO;
      
      DLP_ControlSignal->startTime = os_get_time();
      PAD_ControlSignal->startTime = os_get_time();
      X86_ControlSignal->startTime = os_get_time();
      NV_ControlSignal->startTime = os_get_time();
    
      boardStatus->startTime = os_get_time();
      boardStatus->isPowerOffFinish = NO;
      platform_log("devices start to shutdown");
      boardStatus->sysStatus &= ~(STATE_RUN_BITS);
      boardStatus->sysStatus |= STATE_IS_POWER_OFF;
      SetSerialLedsEffect( LIGHTS_MODE_NOMAL, NULL, 0 );
    }
  }
}

void setModulePowerSignalOnOff( uint8_t module, uint8_t onoff )
{
  switch( module )
  {
  case PAD_MODULE:
    if( onoff )
    {
      if( PAD_ControlSignal->deviceOnOff == DEVICE_POWER_OFF )
      {
        PAD_ControlSignal->isDeviceProcessOver = NO;
        PAD_ControlSignal->startTime = os_get_time();
      }
    }
    else
    {
      if( PAD_ControlSignal->deviceOnOff == DEVICE_POWER_ON )
      {
        PAD_ControlSignal->isDeviceProcessOver = NO;
        PAD_ControlSignal->startTime = os_get_time();
      }
    }
    break;
  case X86_MODULE:
    if( onoff )
    {
      if( X86_ControlSignal->deviceOnOff == DEVICE_POWER_OFF )
      {
        X86_ControlSignal->isDeviceProcessOver = NO;
        X86_ControlSignal->startTime = os_get_time();
      }
    }
    else
    {
      if( X86_ControlSignal->deviceOnOff == DEVICE_POWER_ON )
      {
        X86_ControlSignal->isDeviceProcessOver = NO;
        X86_ControlSignal->startTime = os_get_time();
      }
    }
    break;
  case NV_MODULE:
    if( onoff )
    {
      if( NV_ControlSignal->deviceOnOff == DEVICE_POWER_OFF )
      {
        NV_ControlSignal->isDeviceProcessOver = NO;
        NV_ControlSignal->startTime = os_get_time();
      }
    }
    else
    {
      if( NV_ControlSignal->deviceOnOff == DEVICE_POWER_ON )
      {
        NV_ControlSignal->isDeviceProcessOver = NO;
        NV_ControlSignal->startTime = os_get_time();
      }
    }
    break;
  case DLP_MODULE:
    if( onoff )
    {
      if( DLP_ControlSignal->deviceOnOff == DEVICE_POWER_OFF )
      {
        DLP_ControlSignal->isDeviceProcessOver = NO;
        DLP_ControlSignal->startTime = os_get_time();
      }
    }
    else
    {
      if( DLP_ControlSignal->deviceOnOff == DEVICE_POWER_ON )
      {
        DLP_ControlSignal->isDeviceProcessOver = NO;
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
extern void UploadPowerOffSignal(uint32_t second);
#define POWER_OFF_DELAY_TIME            0//10*1000/SYSTICK_PERIOD
#define GIVE_UP_TIME                    SWITCH_DEBOUNCE_TIME + 2*1000/SYSTICK_PERIOD
static void Switch_Tick( void )
{
#if 0
  
    static uint32_t switch_on_start_time = 0;
    static uint32_t switch_off_start_time = 0;
    static uint32_t power_off_delay_start_time = 0;
    static uint32_t give_up_start_time = 0;
    uint8_t switch_state = 0;
    
    if(STATE_POWER_OFF == (boardStatus->sysStatus & STATE_RUN_BITS) )
    {
        switch_state = switch_user->getSwitchState( SWITCH_USER );

        
        if(switch_state == 0)
        {
            switch_on_start_time = os_get_time();
        }
        if(os_get_time() - switch_on_start_time >= SWITCH_DEBOUNCE_TIME)
        {
            //if((battery_pack.percentage > VBAT_POWER_OFF_PERCENTAGE + 5) || (battery_pack.com_status == false))
            {
                PowerOnDevices();
            }
            
        }  
        
        if(os_get_time() - give_up_start_time >= GIVE_UP_TIME)
        {
            boardStatus->setPowerOnoff(POWER_CTRL_OUT, POWER_OFF);
        }
    }
    
    if(STATE_POWER_ON == (boardStatus->sysStatus & STATE_RUN_BITS) )
    {
        switch_state = switch_user->getSwitchState( SWITCH_USER );
        if(switch_state == 1)
        {
            switch_off_start_time = os_get_time();
        }
        if(power_off_delay_start_time == 0)
        {
            if(os_get_time() - switch_off_start_time >= SWITCH_DEBOUNCE_TIME)
            {
                power_off_delay_start_time = os_get_time();
                UploadPowerOffSignal(POWER_OFF_DELAY_TIME * SYSTICK_PERIOD / 1000);
            }
        }
        
        if(power_off_delay_start_time > 0)
        {
            if(os_get_time() - power_off_delay_start_time >= POWER_OFF_DELAY_TIME)
            {
                PowerOffDevices();
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
        if((boardStatus->isPowerOffFinish == YES) && (boardStatus->isPowerOnFinish == YES))
        {
            if(state_change == 0)
            {
                if( switch_user->preIOState != switch_user->getSwitchState( SWITCH_USER ) )
                {
                    switch_user->preIOState = switch_user->getSwitchState( SWITCH_USER );
                    state_change = 1;
                    platform_log("switch have switched");
                }
            }
            
            if(state_change == 1)
            {
                if( switch_user->preIOState != switch_user->getSwitchState( SWITCH_USER ) )
                {
                    switch_user->preIOState = switch_user->getSwitchState( SWITCH_USER );
                    state_change = 0;
                    switch_user->startTime = 0;
                    return ;
                }
                if(switch_user->startTime == 0)
                {
                    switch_user->startTime = os_get_time();
                }
                if( (switch_user->startTime != 0) && ((os_get_time() - switch_user->startTime) >= SWITCH_DEBOUNCE_TIME) )
                {
                    switch_user->startTime = 0;
    #if 0
                    if( switch_user->preIOState != switch_user->getSwitchState( SWITCH_USER ) )
                    {
                        switch_user->preIOState = switch_user->getSwitchState( SWITCH_USER );
                    }
                    else
                    {
                        return;
                    }
    #endif

                    switch_user->isSwitchOver = YES;
                    if( (STATE_POWER_OFF == (boardStatus->sysStatus & STATE_RUN_BITS))/* && (voltageConvert->bat_voltage >= VBAT_POWER_ON_LEVEL)*/ )
                    {
                        switch_user->switchOnOff = SWITCH_ON;
                        platform_log("confirm to ON stable");
                        if( boardStatus->sysStatus & STATE_IS_LOW_POWER )
                        {
                            boardStatus->sysStatus &= ~STATE_IS_LOW_POWER;
                        }
                        PowerOnDevices();
                        state_change = 0;
                    }
                    
                    else if( ( STATE_POWER_ON == (boardStatus->sysStatus & STATE_RUN_BITS) ) )
                    {
                        switch_user->switchOnOff = SWITCH_OFF;
                        platform_log("confirm to OFF stable");

                        PowerOffDevices();
                        state_change = 0;
                    }
                }     
            }
        }
        else
        {
            switch_user->preIOState = switch_user->getSwitchState( SWITCH_USER );
        }
    }
    else if( DEVICES_ON == boardStatus->devicesOnOff )
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
                        PowerOffDevices();
                        status = REMOTE_POWER_CTRL_STATUS_FINISHED;
                    }
                    else if(boardStatus->remote_device_power_ctrl == REMOTE_DEVICE_POWER_REBOOT)
                    {
                        PowerOffDevices();
                        boardStatus->rebootFlag  = REBOOT_YES;
                        
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

static void ControlSignal_Tick( void )
{

/* pad */
  if( (NO == PAD_ControlSignal->isDeviceProcessOver) && (PAD_ControlSignal->setControlSigal != NULL) )
  {
    if( (DEVICE_POWER_OFF == PAD_ControlSignal->deviceOnOff) && (PAD_ControlSignal->startTime != 0) )
    {
      switch( PAD_ControlSignal->period )
      {
      case SIGNAL_DELAY:
        if( os_get_time() - PAD_ControlSignal->startTime >= PAD_POWER_ON_DELAY_TIME )
        {
          PAD_ControlSignal->startTime = os_get_time();
          PAD_ControlSignal->setControlSigal(POWER_CON_PAD, CONTROL_HOLD);
          PAD_ControlSignal->period = SIGNAL_HOLD;
          platform_log("pad boot signal hold on");
        }
        break;
      case SIGNAL_HOLD:
        if( os_get_time() - PAD_ControlSignal->startTime >= PAD_POWER_ON_HOLD_TIME )
        {
          PAD_ControlSignal->startTime = os_get_time();
          PAD_ControlSignal->setControlSigal(POWER_CON_PAD, CONTROL_RELEASE);
          PAD_ControlSignal->period = SIGNAL_REALSE;
          platform_log("pad boot signal hold off");
        }
        break;
      case SIGNAL_REALSE:
        if( os_get_time() - PAD_ControlSignal->startTime >= PAD_POWER_ON_PROCESSING_TIME )
        {
          PAD_ControlSignal->startTime = 0;
          PAD_ControlSignal->period = SIGNAL_DELAY;
          PAD_ControlSignal->deviceOnOff = DEVICE_POWER_ON;
          PAD_ControlSignal->isDeviceProcessOver = YES;
          platform_log("pad boot have powered on");
        }
        break;
      default:
        break;       
      }
    }
    else if( (DEVICE_POWER_ON == PAD_ControlSignal->deviceOnOff) && (PAD_ControlSignal->startTime != 0) )
    {
      switch( PAD_ControlSignal->period )
      {
      case SIGNAL_DELAY:
        if( os_get_time() - PAD_ControlSignal->startTime >= PAD_POWER_OFF_DELAY_TIME )
        {
          PAD_ControlSignal->startTime = os_get_time();
          PAD_ControlSignal->setControlSigal(POWER_CON_PAD, CONTROL_HOLD);
          PAD_ControlSignal->period = SIGNAL_HOLD;
          platform_log("pad shutdown signal hold on");
        }
        break;
      case SIGNAL_HOLD:
        if( os_get_time() - PAD_ControlSignal->startTime >= PAD_POWER_OFF_HOLD_TIME )
        {
          PAD_ControlSignal->startTime = os_get_time();
          PAD_ControlSignal->setControlSigal(POWER_CON_PAD, CONTROL_RELEASE);
          PAD_ControlSignal->period = SIGNAL_REALSE;
          platform_log("pad shutdown signal hold off");
        }
        break;
      case SIGNAL_REALSE:
        if( os_get_time() - PAD_ControlSignal->startTime >= PAD_POWER_OFF_PROCESSING_TIME )
        {
          PAD_ControlSignal->startTime = 0;
          PAD_ControlSignal->period = SIGNAL_DELAY;
          PAD_ControlSignal->deviceOnOff = DEVICE_POWER_OFF;
          PAD_ControlSignal->isDeviceProcessOver = YES;
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
  if( (DEVICE_POWER_OFF == NV_ControlSignal->deviceOnOff) && (NV_ControlSignal->startTime != 0) )
  {
    switch( NV_ControlSignal->period )
    {
    case SIGNAL_DELAY:
      if( os_get_time() - NV_ControlSignal->startTime >= NV_POWER_ON_DELAY_TIME )
      {
        NV_ControlSignal->startTime = os_get_time();
        NV_ControlSignal->setControlSigal(POWER_CON_NV, CONTROL_HOLD);
        NV_ControlSignal->setControlSigal(POWER_CON_RES, CONTROL_HOLD);//joy test
        NV_ControlSignal->period = SIGNAL_HOLD;
        platform_log("NV boot signal hold on");
      }
      break;

    case SIGNAL_HOLD:
      if( os_get_time() - NV_ControlSignal->startTime >= NV_POWER_ON_HOLD_TIME )
      {
        NV_ControlSignal->startTime = os_get_time();
        NV_ControlSignal->setControlSigal(POWER_CON_NV, CONTROL_RELEASE);
        NV_ControlSignal->setControlSigal(POWER_CON_RES, CONTROL_RELEASE);//joy test
        NV_ControlSignal->period = SIGNAL_REALSE;
        platform_log("NV boot signal hold off");
      }
      break;

    case SIGNAL_REALSE:
      if( os_get_time() - NV_ControlSignal->startTime >= NV_POWER_ON_PROCESSING_TMP_TIME )
      {
        NV_ControlSignal->startTime = 0;
        NV_ControlSignal->period = SIGNAL_DELAY;
        NV_ControlSignal->deviceOnOff = DEVICE_POWER_ON;
        NV_ControlSignal->isDeviceProcessOver = YES;
        platform_log("NV boot have powered on");
      }
      break;
    default:
      break;       
    }
  }
  else if( (DEVICE_POWER_ON == NV_ControlSignal->deviceOnOff) && (NV_ControlSignal->startTime != 0) )
  {
    switch( NV_ControlSignal->period )
    {
    case SIGNAL_DELAY:
      if( os_get_time() - NV_ControlSignal->startTime >= NV_POWER_OFF_DELAY_TIME )
      {
        NV_ControlSignal->startTime = os_get_time();
        NV_ControlSignal->setControlSigal(POWER_CON_NV, CONTROL_HOLD);
        NV_ControlSignal->setControlSigal(POWER_CON_RES, CONTROL_HOLD);//joy test
        NV_ControlSignal->period = SIGNAL_HOLD;
        platform_log("NV shutdown signal hold on");
      }
      break;
    case SIGNAL_HOLD:
      if( os_get_time() - NV_ControlSignal->startTime >= NV_POWER_OFF_HOLD_TMP_TIME )
      {
        NV_ControlSignal->startTime = os_get_time();
        NV_ControlSignal->setControlSigal(POWER_CON_NV, CONTROL_RELEASE);
        NV_ControlSignal->setControlSigal(POWER_CON_RES, CONTROL_RELEASE);//joy test
        NV_ControlSignal->period = SIGNAL_REALSE;
        platform_log("NV shutdown signal hold off");
      }
      break;
    case SIGNAL_REALSE:
      if( os_get_time() - NV_ControlSignal->startTime >= NV_POWER_OFF_PROCESSING_TIME )
      {
        NV_ControlSignal->startTime = 0;
        NV_ControlSignal->period = SIGNAL_DELAY;
        NV_ControlSignal->deviceOnOff = DEVICE_POWER_OFF;
        NV_ControlSignal->isDeviceProcessOver = YES;
        platform_log("NV shutdown have powered off");
      }
      break;
    default:
      break;       
    }
  }
/* x86 */
  if( (NO == X86_ControlSignal->isDeviceProcessOver) && (X86_ControlSignal->setControlSigal != NULL) )
  {
    if( (DEVICE_POWER_OFF == X86_ControlSignal->deviceOnOff) && (X86_ControlSignal->startTime != 0) )
    {
      switch( X86_ControlSignal->period )
      {
      case SIGNAL_DELAY:
        if( os_get_time() - X86_ControlSignal->startTime >= X86_POWER_ON_DELAY_TIME )
        {
          X86_ControlSignal->startTime = os_get_time();
          X86_ControlSignal->setControlSigal(POWER_CON_X86, CONTROL_HOLD);
          X86_ControlSignal->period = SIGNAL_HOLD;
          platform_log("x86 boot signal hold on");
        }
        break;
      case SIGNAL_HOLD:
        if( os_get_time() - X86_ControlSignal->startTime >= X86_POWER_ON_HOLD_TIME )
        {
          X86_ControlSignal->startTime = os_get_time();
          X86_ControlSignal->setControlSigal(POWER_CON_X86, CONTROL_RELEASE);
          X86_ControlSignal->period = SIGNAL_REALSE;
          platform_log("x86 boot signal hold off");
        }
        break;
      case SIGNAL_REALSE:
        if( os_get_time() - X86_ControlSignal->startTime >= X86_POWER_ON_PROCESSING_TIME )
        {
          X86_ControlSignal->startTime = 0;
          X86_ControlSignal->period = SIGNAL_DELAY;
          X86_ControlSignal->deviceOnOff = DEVICE_POWER_ON;
          X86_ControlSignal->isDeviceProcessOver = YES;
          platform_log("x86 boot have powered on");
        }
        break;
      default:
        break;       
      }
    }
    else if( (DEVICE_POWER_ON == X86_ControlSignal->deviceOnOff) && (X86_ControlSignal->startTime != 0) )
    {
      switch( X86_ControlSignal->period )
      {
      case SIGNAL_DELAY:
        if( os_get_time() - X86_ControlSignal->startTime >= X86_POWER_OFF_DELAY_TIME )
        {
          X86_ControlSignal->startTime = os_get_time();
          X86_ControlSignal->setControlSigal(POWER_CON_X86, CONTROL_HOLD);
          X86_ControlSignal->period = SIGNAL_HOLD;
          platform_log("x86 shutdown signal hold on");
        }
        break;
      case SIGNAL_HOLD:
        if( os_get_time() - X86_ControlSignal->startTime >= X86_POWER_OFF_HOLD_TIME )
        {
          X86_ControlSignal->startTime = os_get_time();
          X86_ControlSignal->setControlSigal(POWER_CON_X86, CONTROL_RELEASE);
          X86_ControlSignal->period = SIGNAL_REALSE;
          platform_log("x86 shutdown signal hold off");
        }
        break;
      case SIGNAL_REALSE:
        if( os_get_time() - X86_ControlSignal->startTime >= X86_POWER_OFF_PROCESSING_TIME )
        {
          X86_ControlSignal->startTime = 0;
          X86_ControlSignal->period = SIGNAL_DELAY;
          X86_ControlSignal->deviceOnOff = DEVICE_POWER_OFF;
          X86_ControlSignal->isDeviceProcessOver = YES;
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

static void BoardStatus_Tick( void )
{
  // boot 
  if( (NO == boardStatus->isPowerOnFinish) && (YES == boardStatus->isPowerOffFinish) )
  {
    if( os_get_time() - boardStatus->startTime >= POWER_ON_PROCEESING_TIME )
    {
      boardStatus->startTime = 0;
      boardStatus->isPowerOnFinish = YES;
      boardStatus->devicesOnOff = DEVICES_ON;
      platform_log("all devices boot finished!");
      boardStatus->sysStatus &= ~(STATE_RUN_BITS);
      boardStatus->sysStatus |= STATE_POWER_ON;

      if( serial->rx_info->startTime == 0 )
      {
        //SetSerialLedsEffect( LIGHTS_MODE_ERROR, NULL, 0 );
      }
      else
      {
        SetSerialLedsEffect( LIGHTS_MODE_NOMAL, NULL, 0 );
      }

#ifdef JOY_TEST
      boardStatus->rebootFlag = 1;
      PowerOffDevices();
#endif
    }
  }
  //shutdown
  if( (NO == boardStatus->isPowerOffFinish) && (YES == boardStatus->isPowerOnFinish) )
  {
    if( os_get_time() - boardStatus->startTime >= POWER_OFF_PROCEESING_TIME )
    {
      boardStatus->startTime = 0;
      boardStatus->isPowerOffFinish = YES;
      boardStatus->setPowerOnoff(POWER_ALL, POWER_OFF);
      boardStatus->devicesOnOff = DEVICES_OFF;
      platform_log("all devices shutdown finished");
      boardStatus->sysStatus &= ~(STATE_RUN_BITS);
      boardStatus->sysStatus |= STATE_POWER_OFF;
      if( ( REBOOT_YES == boardStatus->rebootFlag )
         && (voltageConvert->bat_voltage >= VBAT_POWER_ON_LEVEL) )
      {       
        boardStatus->startTime = os_get_time();
        //platform_log("board start to reboot after 5 seconds");
        platform_log("board start to reboot right now");
        flashTable.isNeedAutoBoot = 'Y';
        flashTable.AutoBootDelayTime = 10;
        MICOBootConfiguration( &flashTable );
        NVIC_SystemReset();
      }
      else
      {
        NVIC_SystemReset();
        boardStatus->sysStatus |= SYSTEM_IS_SLEEP;
        halEnterSleepMode();
      }      
    }
  }
  if( (REBOOT_YES == boardStatus->rebootFlag) && (YES == boardStatus->isPowerOffFinish) )
  {
    if( os_get_time() - boardStatus->startTime >= BOARD_REBOOT_WAITING_TIME )
    {
      PowerOnDevices();
      boardStatus->rebootFlag = REBOOT_NO;
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


uint32_t GetEachModuleStates( void )
{
  uint32_t states;
  
  states = GetModulePowerState( POWER_ALL );
#if 0
  if( SWITCH_ON == switch_user->switchOnOff )
  {
      states |= SWITCH_STATE_ON;
  }
  //if( DEVICE_POWER_ON == DLP_ControlSignal->deviceOnOff )
  {
      //states |= POWER_DLP;
  }
  if( DEVICE_POWER_ON == X86_ControlSignal->deviceOnOff )
  {
      states |= POWER_X86;
  }
  if( DEVICE_POWER_ON == PAD_ControlSignal->deviceOnOff )
  {
      states |= POWER_PAD;
  }
  if( DEVICE_POWER_ON == NV_ControlSignal->deviceOnOff )
  {
      states |= POWER_NV;
  }
#endif
  return states;
}

extern OSStatus AckReadSysStatusFrameProcess( serial_t *serial, uint8_t cmd_num );
extern void UploadSysState(void);
void ChargerPlugInCallback(void)
{
    if(MicoGpioInputGet( MICO_GPIO_CHARGE_IN ) == 1)
    {
        boardStatus->sysStatus |= STATE_IS_CHARGER_IN;
    }
    else
    {
        boardStatus->sysStatus &= ~STATE_IS_CHARGER_IN;
    }
    if(MicoGpioInputGet( MICO_GPIO_RECHARGE_IN ) == 1)
    {
        boardStatus->sysStatus |= STATE_IS_RECHARGE_IN;
    }
    else
    {
        boardStatus->sysStatus &= ~STATE_IS_RECHARGE_IN;
    }
    
    if( (MicoGpioInputGet( MICO_GPIO_CHARGE_IN ) == 0) && (MicoGpioInputGet( MICO_GPIO_RECHARGE_IN ) == 0) )
    {
        boardStatus->sysStatus &= ~STATE_IS_CHARGER_IN;
        boardStatus->sysStatus &= ~STATE_IS_RECHARGE_IN;
        boardStatus->sysStatus &= ~STATE_IS_CHARGING;
    }
    else
    {
      if(battery_pack.pack_voltage < 100)  
      {
        boardStatus->sysStatus |= STATE_IS_CHARGING;
      } 
    }
    //AckReadSysStatusFrameProcess(serial, 0);
    
    UploadSysState();
    
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_CHARGE_FAN_CTRL);//charger plug in fan ctrl
}

void ChargerUnplugCallback(void)
{
    if(MicoGpioInputGet( MICO_GPIO_CHARGE_IN ) == 1)
    {
        boardStatus->sysStatus |= STATE_IS_CHARGER_IN;
    }
    else
    {
        boardStatus->sysStatus &= ~STATE_IS_CHARGER_IN;
    }
    if(MicoGpioInputGet( MICO_GPIO_RECHARGE_IN ) == 1)
    {
        boardStatus->sysStatus |= STATE_IS_RECHARGE_IN;
    }
    else
    {
        boardStatus->sysStatus &= ~STATE_IS_RECHARGE_IN;
    }
    
    if( (MicoGpioInputGet( MICO_GPIO_CHARGE_IN ) == 0) && (MicoGpioInputGet( MICO_GPIO_RECHARGE_IN ) == 0) )
    {
        boardStatus->sysStatus &= ~STATE_IS_CHARGER_IN;
        boardStatus->sysStatus &= ~STATE_IS_RECHARGE_IN;
        boardStatus->sysStatus &= ~STATE_IS_CHARGING;
    }
    else
    {
      if(battery_pack.pack_voltage < 100)  
      {
        boardStatus->sysStatus |= STATE_IS_CHARGING;
      }   
    }
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_CHARGE_FAN_CTRL);//charger unplug fan ctrl
    UploadSysState();
}
#define CHARGING_DEBAUNCE_TIME  20/SYSTICK_PERIOD
void ChargeTick(void)
{
    extern const platform_gpio_t            platform_gpio_pins[];  
    static uint8_t state = 0;
    static uint32_t plug_in_start_time = 0;
    
    switch(state)
    {
    case 0:
      if( (MicoGpioInputGet( MICO_GPIO_CHARGE_IN ) == 1) || (MicoGpioInputGet( MICO_GPIO_RECHARGE_IN ) == 1) )
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
            ChargerPlugInCallback();
               
      }
      break;
    case 2:
      if( (MicoGpioInputGet( MICO_GPIO_CHARGE_IN ) == 0) && (MicoGpioInputGet( MICO_GPIO_RECHARGE_IN ) == 0) )
      {
            state = 0;
            platform_log("charge port not plug in\r\n");
            ChargerUnplugCallback();            
            
      }
    default :
      break;
    }
    
    if( (MicoGpioInputGet( MICO_GPIO_CHARGE_IN ) == 0) && (MicoGpioInputGet( MICO_GPIO_RECHARGE_IN ) == 0) )
    {
        state = 0;
        boardStatus->sysStatus &= ~STATE_IS_CHARGER_IN;
        boardStatus->sysStatus &= ~STATE_IS_RECHARGE_IN;
        boardStatus->sysStatus &= ~STATE_IS_CHARGING;
    }
    else
    {
#if 0
      if(battery_pack.percentage < 100)  
      {
        boardStatus->sysStatus |= STATE_IS_CHARGING;
      }
      else if(battery_pack.percentage == 100) 
      {
        boardStatus->sysStatus &= ~STATE_IS_CHARGING;
      }
#endif
      if(battery_pack.pack_current > 0)
      {
        boardStatus->sysStatus |= STATE_IS_CHARGING;
      }
      else
      {
        boardStatus->sysStatus &= ~STATE_IS_CHARGING;
      }
        
    }
    
}
void Platform_Tick( void )
{
  Switch_Tick();
  ControlSignal_Tick();
  BoardStatus_Tick();
  ChargeTick();
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
void bsp_Init( void )
{
  board_gpios_init();
  charger_detect_init();
  //key_switch_init();
}

void key_switch_interrupt_cb( void )
{
  if( boardStatus->sysStatus & SYSTEM_IS_SLEEP )
  {
    boardStatus->sysStatus &= ~(SYSTEM_IS_SLEEP);
    halWakeupFormSLeep();       
  }   
  switch_user->isSwitchOver = NO;
  switch_user->startTime = os_get_time();
  platform_log("switch have switched");
}
/*********************END OF FILE**************/
