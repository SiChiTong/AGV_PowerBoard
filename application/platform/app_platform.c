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

void PowerOnDevices( void )
{
  if( DEVICES_OFF == boardStatus->devicesOnOff )
  {
    if( (YES == boardStatus->isPowerOffFinish) && (YES == boardStatus->isPowerOnFinish) )
    {
      boardStatus->startTime = os_get_time();
      boardStatus->isPowerOnFinish = NO;
      boardStatus->setPowerOnoff(POWER_ALL, POWER_ON);
      
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
      setCurLedsMode(LIGHTS_MODE_WELCOME);
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
      setCurLedsMode(LIGHTS_MODE_GOODEBYE);
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

static void Switch_Tick( void )
{
  if( /*(YES != switch_user->isSwitchOver) && */(switch_user->getSwitchState != NULL) )
  {
    if( (switch_user->startTime != 0) && ((os_get_time() - switch_user->startTime) >= SWITCH_DEBOUNCE_TIME) )
    {
      switch_user->startTime = 0;
      
      if( switch_user->preIOState != switch_user->getSwitchState( SWITCH_USER ) )
      {
        switch_user->preIOState = switch_user->getSwitchState( SWITCH_USER );
      }
      else
      {
        return;
      }

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
      }
      if( ( STATE_POWER_ON == (boardStatus->sysStatus & STATE_RUN_BITS) ) )
      {
          switch_user->switchOnOff = SWITCH_OFF;
          platform_log("confirm to OFF stable");
          
          PowerOffDevices();
      }
    }
  }
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
        setCurLedsMode(LIGHTS_MODE_COM_FAULT);
      }
      else
      {
        setCurLedsMode(LIGHTS_MODE_IDLE);
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

#ifdef NOT_USE_TMP
uint32_t getEachModuleStates( void )
{
  uint32_t states;
  
  states = getModulePowerState( POWER_ALL );
  if( SWITCH_ON == switch_user->switchOnOff )
  {
      states |= SWITCH_STATE_ON;
  }
  if( DEVICE_POWER_ON == DLP_ControlSignal->deviceOnOff )
  {
      states |= POWER_DLP;
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
  return states;
}
#endif
void Platform_Tick( void )
{
  Switch_Tick();
  ControlSignal_Tick();
  BoardStatus_Tick();
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
