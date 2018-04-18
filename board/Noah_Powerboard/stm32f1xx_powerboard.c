/* 
*  Author: Adam Huang 
*  Date:2016/6/4
*/

#include "stm32f1xx_powerboard.h"
#include "Mico.h"
 
uint32_t BSP_SW_GetState(Switch_TypeDef Switch)
{
  (void)Switch;    
  return MicoGpioInputGet( MICO_GPIO_PWRKEY );
}

void BSP_Power_OnOff(PowerEnable_TypeDef PowerEn, PowerOnOff_TypeDef OnOff)
{
  if( POWER_ON == OnOff )
  {
    if( PowerEn & POWER_5V_MOTOR )
    {
      MicoGpioOutputLow( MICO_GPIO_5V_MOTOR_EN );
    }
    if( PowerEn & POWER_5V_RECHARGE )
    {
      MicoGpioOutputLow( MICO_GPIO_5V_RECHARGE_EN );
    }
    if( PowerEn & POWER_5V_SENSOR_BOARD      )
    {
      MicoGpioOutputLow( MICO_GPIO_5V_SENSOR_BOARD_EN );
    }
    if( PowerEn & POWER_5V_SWITCH )
    {
      MicoGpioOutputLow( MICO_GPIO_5V_SWITCH_EN );
    }
    if( PowerEn & POWER_5V_ROUTER  )
    {
      MicoGpioOutputLow( MICO_GPIO_5V_ROUTER_EN   );
    }
    if( PowerEn & POWER_5V_EN )
    {
      MicoGpioOutputLow( MICO_GPIO_5V_EN );
    }
    if( PowerEn & POWER_12V_PAD )
    {
      MicoGpioOutputLow( MICO_GPIO_12V_PAD_EN );
    }
    if( PowerEn & POWER_12V_2_1_PA )
    {
      MicoGpioOutputLow( MICO_GPIO_12V_2_1_PA_EN );
    }
    if( PowerEn & POWER_12V_EXTEND )
    {
      MicoGpioOutputLow( MICO_GPIO_12V_EXTEND_EN );
    }
    if( PowerEn & POWER_12V_X86 )
    {
      MicoGpioOutputLow( MICO_GPIO_12V_X86_EN );
    }
    if( PowerEn & POWER_12V_NV )
    {
      MicoGpioOutputLow( MICO_GPIO_12V_NV_EN );
    }
    if( PowerEn & POWER_12V_EN )
    {
      MicoGpioOutputLow( MICO_GPIO_12V_EN );
    }
    if( PowerEn & POWER_24V_EN )
    {
      MicoGpioOutputLow( MICO_GPIO_24V_EN );
    }
    if( PowerEn & POWER_24V_PRINTER )
    {
      MicoGpioOutputLow( MICO_GPIO_24V_PRINTER_EN    );
    }
    if( PowerEn & POWER_24V_EXTEND )
    {
      MicoGpioOutputLow( MICO_GPIO_24V_EXTEND_EN   );
    }
    if( PowerEn & POWER_VSYS_24V_NV )
    {
      MicoGpioOutputLow( MICO_GPIO_VSYS_24V_NV_EN );
    }
    if( PowerEn & POWER_485 )
    {
      MicoGpioOutputLow( MICO_GPIO_485_EN );
    }
    if( PowerEn & POWER_RECHARGE_LED )
    {
      MicoGpioOutputLow( MICO_GPIO_RECHARGE_LED );
    }
    if( PowerEn & POWER_SLAM )
    {
      MicoGpioOutputLow( MICO_GPIO_SLAM_EN);
    }
    
    if( PowerEn & POWER_LED_MCU )
    {
      MicoGpioOutputLow( MICO_GPIO_LED_MCU_POWER_EN);
    }
    
    
    
    
    
  /*  
    if( PowerEn & POWER_CHARGE_FAN )
    {
      MicoGpioOutputHigh( MICO_GPIO_CHARGE_FAN_CTRL);
    }
    */
    if( PowerEn & POWER_POLE_MOTOR )
    {
      MicoGpioOutputLow( MICO_GPIO_5V_POLE_MOTOR_EN);
    }
    if( PowerEn & POWER_5V_KEYPAD )
    {
      MicoGpioOutputLow( MICO_GPIO_5V_KEYPAD_EN);
    }
    
    if( PowerEn & POWER_CAMERA_FRONT_LED )
    {
      MicoGpioOutputHigh( MICO_GPIO_CAMERA_FRONT_LED_EN);
    }
    if( PowerEn & POWER_CAMERA_BACK_LED )
    {
      MicoGpioOutputHigh( MICO_GPIO_CAMERA_BACK_LED_EN);
    }
    
    if( PowerEn & POWER_CTRL_OUT )
    {
      MicoGpioOutputHigh( MICO_GPIO_PWR_CTRL_OUT);
    }
    
    if( PowerEn & POWER_DOOR_CTRL )
    {
      MicoGpioOutputHigh( MICO_GPIO_3_3V_DOOR_CTRL);
    }
   
  }
  else if( POWER_OFF == OnOff )
  {
     if( PowerEn & POWER_5V_MOTOR )
    {
      MicoGpioOutputHigh( MICO_GPIO_5V_MOTOR_EN );
    }
    if( PowerEn & POWER_5V_RECHARGE )
    {
      MicoGpioOutputHigh( MICO_GPIO_5V_RECHARGE_EN );
    }
    if( PowerEn & POWER_5V_SENSOR_BOARD      )
    {
      MicoGpioOutputHigh( MICO_GPIO_5V_SENSOR_BOARD_EN );
    }
    if( PowerEn & POWER_5V_SWITCH )
    {
      MicoGpioOutputHigh( MICO_GPIO_5V_SWITCH_EN );
    }
    if( PowerEn & POWER_5V_ROUTER  )
    {
      MicoGpioOutputHigh( MICO_GPIO_5V_ROUTER_EN   );
    }
    if( PowerEn & POWER_5V_EN )
    {
      MicoGpioOutputHigh( MICO_GPIO_5V_EN );
    }
    if( PowerEn & POWER_12V_PAD )
    {
      MicoGpioOutputHigh( MICO_GPIO_12V_PAD_EN );
    }
    if( PowerEn & POWER_12V_2_1_PA )
    {
      MicoGpioOutputHigh( MICO_GPIO_12V_2_1_PA_EN );
    }
    if( PowerEn & POWER_12V_EXTEND )
    {
      MicoGpioOutputHigh( MICO_GPIO_12V_EXTEND_EN );
    }
    if( PowerEn & POWER_12V_X86 )
    {
      MicoGpioOutputHigh( MICO_GPIO_12V_X86_EN );
    }
    if( PowerEn & POWER_12V_NV )
    {
      MicoGpioOutputHigh( MICO_GPIO_12V_NV_EN );
    }
    if( PowerEn & POWER_12V_EN )
    {
      MicoGpioOutputHigh( MICO_GPIO_12V_EN );
    }
    if( PowerEn & POWER_24V_EN )
    {
      MicoGpioOutputHigh( MICO_GPIO_24V_EN );
    }
    if( PowerEn & POWER_24V_PRINTER )
    {
      MicoGpioOutputHigh( MICO_GPIO_24V_PRINTER_EN    );
    }
    if( PowerEn & POWER_24V_EXTEND )
    {
      MicoGpioOutputHigh( MICO_GPIO_24V_EXTEND_EN   );
    }
    if( PowerEn & POWER_VSYS_24V_NV )
    {
      MicoGpioOutputHigh( MICO_GPIO_VSYS_24V_NV_EN );
    }
    if( PowerEn & POWER_485 )
    {
      MicoGpioOutputHigh( MICO_GPIO_485_EN );
    }
    if( PowerEn & POWER_RECHARGE_LED )
    {
      MicoGpioOutputHigh( MICO_GPIO_RECHARGE_LED );
    }
    if( PowerEn & POWER_SLAM )
    {
      MicoGpioOutputHigh( MICO_GPIO_SLAM_EN);
    }
    
    if( PowerEn & POWER_LED_MCU )
    {
      MicoGpioOutputHigh( MICO_GPIO_LED_MCU_POWER_EN);
    }
    
    
    
   /* 
    if( PowerEn & POWER_CHARGE_FAN )
    {
      MicoGpioOutputLow( MICO_GPIO_CHARGE_FAN_CTRL);
    }
*/
    
    if( PowerEn & POWER_POLE_MOTOR )
    {
      MicoGpioOutputHigh( MICO_GPIO_5V_POLE_MOTOR_EN);
    }
    if( PowerEn & POWER_5V_KEYPAD )
    {
      MicoGpioOutputHigh( MICO_GPIO_5V_KEYPAD_EN);
    }
    
    if( PowerEn & POWER_CTRL_OUT )
    {
      MicoGpioOutputLow( MICO_GPIO_PWR_CTRL_OUT);
    }
    
    if( PowerEn & POWER_CAMERA_FRONT_LED )
    {
      MicoGpioOutputLow( MICO_GPIO_CAMERA_FRONT_LED_EN);
    }
    if( PowerEn & POWER_CAMERA_BACK_LED )
    {
      MicoGpioOutputLow( MICO_GPIO_CAMERA_BACK_LED_EN);
    }
    if( PowerEn & POWER_DOOR_CTRL )
    {
      MicoGpioOutputLow( MICO_GPIO_3_3V_DOOR_CTRL);
    }
    
  }
}

uint32_t GetModulePowerState( PowerEnable_TypeDef PowerEn )
{
    volatile uint32_t pinState;
    
    pinState = (uint32_t)0;

    if( PowerEn & POWER_5V_MOTOR )
    {
      if( !MicoGpioInputGet( MICO_GPIO_5V_MOTOR_EN ) )
      {
        pinState |= POWER_5V_MOTOR;
      }
    }
    if( PowerEn & POWER_5V_RECHARGE )
    {
      if( !MicoGpioInputGet( MICO_GPIO_5V_RECHARGE_EN ) )
      {
        pinState |= POWER_5V_RECHARGE;
      }
    }
    if( PowerEn & POWER_5V_SENSOR_BOARD      )
    {
      if( !MicoGpioInputGet( MICO_GPIO_5V_SENSOR_BOARD_EN ) )
      {
        pinState |= POWER_5V_SENSOR_BOARD     ;
      }
    }
    if( PowerEn & POWER_5V_SWITCH )
    {
      if( !MicoGpioInputGet( MICO_GPIO_5V_SWITCH_EN ) )
      {
        pinState |= POWER_5V_SWITCH;
      }
    }
    if( PowerEn & POWER_5V_ROUTER  )
    {
      if( !MicoGpioInputGet( MICO_GPIO_5V_ROUTER_EN   ) )
      {
        pinState |= POWER_5V_ROUTER ;
      }
    }
    if( PowerEn & POWER_5V_EN )
    {
      if( !MicoGpioInputGet( MICO_GPIO_5V_EN ) )
      {
        pinState |= POWER_5V_EN;
      }
    }
    if( PowerEn & POWER_12V_PAD )
    {
      if( !MicoGpioInputGet( MICO_GPIO_12V_2_1_PA_EN ) )
      {
        pinState |= POWER_12V_PAD;
      }
    }
    if( PowerEn & POWER_12V_2_1_PA )
    {
      if( !MicoGpioInputGet( MICO_GPIO_12V_PAD_EN ) )
      {
        pinState |= POWER_12V_2_1_PA;
      }
    }
    if( PowerEn & POWER_12V_EXTEND )
    {
      if( !MicoGpioInputGet( MICO_GPIO_12V_EXTEND_EN ) )
      {
        pinState |= POWER_12V_EXTEND;
      }
    }
    if( PowerEn & POWER_12V_X86 )
    {
      if( !MicoGpioInputGet( MICO_GPIO_12V_X86_EN ) )
      {
        pinState |= POWER_12V_X86;
      }
    }
    if( PowerEn & POWER_12V_NV )
    {
      if( !MicoGpioInputGet( MICO_GPIO_12V_NV_EN ) )
      {
        pinState |= POWER_12V_NV;
      }
    }
    if( PowerEn & POWER_12V_EN )
    {
      if( !MicoGpioInputGet( MICO_GPIO_12V_EN ) )
      {
        pinState |= POWER_12V_EN;
      }
    }
    if( PowerEn & POWER_24V_EN )
    {
      if( !MicoGpioInputGet( MICO_GPIO_24V_EN ) )
      {
        pinState |= POWER_24V_EN;
      }
    }
    if( PowerEn & POWER_24V_PRINTER )
    {
      if( !MicoGpioInputGet( MICO_GPIO_24V_PRINTER_EN))
      {
        pinState |= POWER_24V_PRINTER;
      }
    }
 
    if( PowerEn & POWER_24V_EXTEND )
    {
      if( !MicoGpioInputGet( MICO_GPIO_24V_EXTEND_EN   ) )
      {
        pinState |= POWER_24V_EXTEND;
      }
    }
    if( PowerEn & POWER_VSYS_24V_NV )
    {
      if( !MicoGpioInputGet( MICO_GPIO_VSYS_24V_NV_EN ) )
      {
        pinState |= POWER_VSYS_24V_NV;
      }
    }
    if( PowerEn & POWER_485 )
    {
      if( !MicoGpioInputGet( MICO_GPIO_485_EN ) )
      {
        pinState |= POWER_485;
      }
    }
    if( PowerEn & POWER_SYS_LED )
    {
      if( !MicoGpioInputGet( MICO_GPIO_SYS_LED ) )
      {
        pinState |= POWER_SYS_LED;
      }
    }
    if( PowerEn & POWER_RECHARGE_LED )
    {
      if( !MicoGpioInputGet( MICO_GPIO_RECHARGE_LED ) )
      {
        pinState |= POWER_RECHARGE_LED;
      }
    }
    if( PowerEn & POWER_SLAM )
    {
      if( !MicoGpioInputGet( MICO_GPIO_SLAM_EN  ) )
      {
        pinState |= POWER_SLAM;
      }
    }
    
    
    if( PowerEn & POWER_LED_MCU )
    {
      if( !MicoGpioInputGet( MICO_GPIO_LED_MCU_POWER_EN  ) )
      {
        pinState |= POWER_LED_MCU;
      }
    }
      if( PowerEn & POWER_CHARGE_FAN )
    {
      if( !MicoGpioInputGet( MICO_GPIO_CHARGE_FAN_CTRL  ) )
      {
        pinState |= POWER_CHARGE_FAN;
      }
    }
    if( PowerEn & POWER_POLE_MOTOR )
    {
      if( !MicoGpioInputGet( MICO_GPIO_5V_POLE_MOTOR_EN  ) )
      {
        pinState |= POWER_POLE_MOTOR;
      }
    }
    
    if( PowerEn & POWER_5V_KEYPAD )
    {
      if( !MicoGpioInputGet( MICO_GPIO_5V_KEYPAD_EN  ) )
      {
        pinState |= POWER_5V_KEYPAD;
      }
    }
    if( PowerEn & POWER_CAMERA_FRONT_LED )
    {
      if( MicoGpioInputGet( MICO_GPIO_CAMERA_FRONT_LED_EN  ) )
      {
        pinState |= POWER_CAMERA_FRONT_LED;
      }
    }
    if( PowerEn & POWER_CAMERA_BACK_LED )
    {
      if( MicoGpioInputGet( MICO_GPIO_CAMERA_BACK_LED_EN  ) )
      {
        pinState |= POWER_CAMERA_BACK_LED;
      }
    }
    
    if( PowerEn & POWER_CTRL_OUT )
    {
      if( MicoGpioInputGet( MICO_GPIO_PWR_CTRL_OUT  ) )
      {
        pinState |= POWER_CTRL_OUT;
      }
    }
    
    if( PowerEn & POWER_DOOR_CTRL )
    {
      if( MicoGpioInputGet( MICO_GPIO_3_3V_DOOR_CTRL  ) )
      {
        pinState |= POWER_DOOR_CTRL;
      }
    }
    return pinState;
}

void BSP_Control_Sigal(PowerControl_TypeDef PowerCon, ControlSignal_TypeDef isHold)
{
  if( CONTROL_RELEASE == isHold )
  {
#if 1
    switch( PowerCon )
    {
    case POWER_CON_NV:
      MicoGpioOutputLow( MICO_GPIO_PWR_NV ); 
      break;
//    case POWER_CON_DLP:
//      MicoGpioOutputLow( MICO_GPIO_PWR_DLP ); 
//      break;
    case POWER_CON_PAD:
      MicoGpioOutputLow( MICO_GPIO_PWR_PAD ); 
      break;
    case POWER_CON_X86:
      MicoGpioOutputLow( MICO_GPIO_PWR_X86 ); 
      break;
    case POWER_CON_RES:
      MicoGpioOutputLow( MICO_GPIO_PWR_RESERVE );
      break;
    default:
      break;
    }
#else
    if( PowerCon & POWER_CON_NV )
    {
      MicoGpioOutputLow( MICO_GPIO_PWR_NV ); 
    }
    if( PowerCon & POWER_CON_DLP )
    {
      MicoGpioOutputLow( MICO_GPIO_PWR_DLP ); 
    }
    if( PowerCon & POWER_CON_PAD )
    {
      MicoGpioOutputLow( MICO_GPIO_PWR_PAD ); 
    }
    if( PowerCon & POWER_CON_X86 )
    {
      MicoGpioOutputLow( MICO_GPIO_PWR_X86 ); 
    }
    if( PowerCon & POWER_CON_RES )
    {
      MicoGpioOutputLow( MICO_GPIO_PWR_RES ); 
    }
#endif
  }
  else if( CONTROL_HOLD == isHold )
  {
#if 1
    switch( PowerCon )
    {
    case POWER_CON_NV:
      MicoGpioOutputHigh( MICO_GPIO_PWR_NV ); 
      break;
//    case POWER_CON_DLP:
//      MicoGpioOutputHigh( MICO_GPIO_PWR_DLP ); 
//      break;
    case POWER_CON_PAD:
      MicoGpioOutputHigh( MICO_GPIO_PWR_PAD ); 
      break;
    case POWER_CON_X86:
      MicoGpioOutputHigh( MICO_GPIO_PWR_X86 ); 
      break;
    case POWER_CON_RES:
      MicoGpioOutputHigh( MICO_GPIO_PWR_RESERVE );
      break;
    default:
      break;
    }
#else
    if( PowerCon & POWER_CON_NV )
    {
      MicoGpioOutputHigh( MICO_GPIO_PWR_NV ); 
    }
    if( PowerCon & POWER_CON_DLP )
    {
      MicoGpioOutputHigh( MICO_GPIO_PWR_DLP ); 
    }
    if( PowerCon & POWER_CON_PAD )
    {
      MicoGpioOutputHigh( MICO_GPIO_PWR_PAD ); 
    }
    if( PowerCon & POWER_CON_X86 )
    {
      MicoGpioOutputHigh( MICO_GPIO_PWR_X86 ); 
    }
    if( PowerCon & POWER_CON_RES )
    {
      MicoGpioOutputHigh( MICO_GPIO_PWR_RES ); 
    }
#endif
  }
}

#ifndef BOOTLOADER
void halEnterSleepMode( void )
{
//  BSP_LED_Off( LED_SYS );
//  HAL_PWR_EnterSTANDBYMode();
}

void halWakeupFormSLeep( void )
{
//  init_clocks();
}
#endif

