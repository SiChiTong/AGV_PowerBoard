/* 
*  Author: Adam Huang
*  Date:2016/6/8
*/
#include "voltage_detect.h"
#include <stdlib.h>
#include "stm32f1xx_powerboard.h"
#include "app_platform.h"
#include "serial_leds.h"
#include "protocol.h"
#include "multi_channel_detection.h"
#include "temp_reference.h"
#include "battery.h"

#define vol_detect_log(M, ...) custom_log("VolDetect", M, ##__VA_ARGS__)
#define vol_detect_log_trace() custom_log_trace("VolDetect")

#define BAT_RES                0.1 //unit: ¦¸

static voltageData_t ramVoltageConvert;
voltageData_t *voltageConvert = NULL;//&ramVoltageConvert;

#ifdef VOLTAGE_DEBUG
static voltageData_t ramTempMaxVoltageData;
static voltageData_t *tempMaxVoltageData = NULL;
#endif

voltageConvertData_t *voltageConvertData;
voltageDebug_t voltageDebug;

static void computeVoltage( void );

typedef enum {
#ifdef     ADC_TEST_TMP
  CURRENTS_5V_RES = 0,
  CURRENTS_12V_RES,
  CURRENTS_BAT_NV,
  CURRENTS_12V_NV,
  CURRENTS_ROUTER_12V,
  CURRENTS_DYP,
  CURRENTS_SENSOR,
  CURRENTS_DLP,
  CURRENTS_MOTOR,
  CURRENTS_24V_RES,
  CURRENTS_2_1_PA,
  CURRENTS_PAD,
  CURRENTS_PRINTER,
  CURRENTS_X86,
  CURRENTS_IRLED,
  CURRENTS_LEDS,
  CURRENTS_CHARGE,
  CURRENTS_BATIN,
  CURRENTS_VBUS,
  CURRENTS_BAT_MOTOR,
  TEMP_24V_TS,
  TEMP_12V_TS,
  TEMP_5V_TS,
  TEMP_AIR_TS,
  CURRENTS_24V_ALL,
  CURRENTS_12V_ALL,
  CURRENTS_5V_ALL,
  VOLTAGE_24V,
  VOLTAGE_12V,
  VOLTAGE_5V,
  VOLTAGE_BAT,
  CURRENTS_AIUI,
  CURRENTS_ROUTER_5V,
#else
  //ADC 1
    CURRENTS_5V_RES = 0,
    //CURRENTS_24V_NV,
    //CURRENTS_12V_NV,
    CURRENTS_48V_EXTEND,
    
    CURRENTS_12V_EXTEND,
    CURRENTS_5V_MOTOR,
    CURRENTS_24V_SLAM,
    CURRENTS_12V_2_1_PA,
    
    CURRENTS_12V_PAD,
    CURRENTS_24V_PRINTER,
    CURRENTS_12V_X86,
    CURRENTS_IRLED,

    
    CURRENTS_5V_LEDS,
    
 //ADC 3   
    CURRENTS_RECHARGE,
    CURRENTS_24V_EXTEND,
    CURRENTS_CHARGE,
    CURRENTS_BATIN,
    CURRENTS_VBUS,
    CURRENTS_BAT_MOTOR,
    ADC_MULTI_CHANNEL,
    

    TEMP_24V_TS,
    TEMP_12V_TS,
    TEMP_5V_TS,
    TEMP_AIR_TS,
    
    CURRENTS_24V_ALL,
    CURRENTS_12V_ALL,
    CURRENTS_5V_ALL,
    VOLTAGE_24V,
    
    VOLTAGE_12V,
    VOLTAGE_5V,
    VOLTAGE_BAT,
    CURRENTS_SENSOR_BOARD,
    
    CURRENTS_12V_ROUTER,
    CURRENTS_24V_NV,
    CURRENTS_12V_NV,
    CURRENTS_KEYPAD,
    

#endif
} adc_channel_t ;

#define ADC_FACTOR_5    5
#define ADC_FACTOR_50    50
struct convert_adc_data convert_data[] = {
  [CURRENTS_5V_RES] = 
  {
    .adc_type           = MICO_ADC_5V_RESERVE_C   ,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = 5,//6.05,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x01,
    .fault_bit_mask_num = RES_5V_CURRENTS_FAULT_BIT_MASK_NUM,
  },
  
  
  
  
  
  
  
  
  
  
  //DH12V
  //DH5V
  [CURRENTS_48V_EXTEND] = 
  {
    .adc_type           = MICO_ADC_48V_EXTEND_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_50,//5.55,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x07,
    .fault_bit_mask_num = SENSOR_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_12V_EXTEND] = 
  {
    .adc_type           = MICO_ADC_12V_EXTEND_C   ,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_5,//6.2,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x08,
    .fault_bit_mask_num = DLP_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_5V_MOTOR] = 
  {
    .adc_type           = MICO_ADC_5V_MOTOR_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_5,//5.8,//
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x09,
    .fault_bit_mask_num = MOTOR_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_24V_SLAM] = 
  {
    .adc_type           = MICO_ADC_24V_SLAM_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_5,//4.985,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x0A,
    .fault_bit_mask_num = RES_24V_CURRENTS_FAULT_BIT_MASK_NUM, 
  }, 
  [CURRENTS_12V_2_1_PA] = 
  {
    .adc_type           = MICO_ADC_12V_2_1_PA_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_5,//5.59,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x0B,
    .fault_bit_mask_num = PA_2_1_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_12V_PAD] = 
  {
    .adc_type           = MICO_ADC_12V_PAD_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_5,//6.3,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x0C,
    .fault_bit_mask_num = PAD_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_24V_PRINTER] = 
  {
    .adc_type           = MICO_ADC_24V_PRINTER_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_5,//5.56,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x0D,
    .fault_bit_mask_num = PRINTER_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_12V_X86] = 
  {
    .adc_type           = MICO_ADC_12V_X86_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_5,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x0E,
    .fault_bit_mask_num = X86_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_IRLED] = 
  {
    .adc_type           = MICO_ADC_IRLED_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_5,//41.17,//
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x0F,
    .fault_bit_mask_num = BAT_MOTOR_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_5V_LEDS] = 
  {
    .adc_type           = MICO_ADC_5V_LEDS_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_5,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x10,
    .fault_bit_mask_num = LEDS_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_RECHARGE] = 
  {
    .adc_type           = MICO_ADC_RECHARGE_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_5,//5.65,//
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x11,
    .fault_bit_mask_num = BAT_NV_CURRENTS_FAULT_BIT_MASK_NUM,    
  },
  [CURRENTS_24V_EXTEND] = 
  {
    .adc_type           = MICO_ADC_24V_EXTEND_C ,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_5,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x12,
    .fault_bit_mask_num = _12V_NV_CURRENTS_FAULT_BIT_MASK_NUM,    
  },
  [CURRENTS_CHARGE] = 
  {
    .adc_type           = MICO_ADC_CHARGE_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_50,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x13,
    .fault_bit_mask_num = ROUTER_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_BATIN] = 
  {
    .adc_type           = MICO_ADC_BATIN_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_5,//7.25,//
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x14,
    .fault_bit_mask_num = DYP_CURRENTS_FAULT_BIT_MASK_NUM, 
  },  
  [CURRENTS_VBUS] = 
  {
    .adc_type           = MICO_ADC_VBUS_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_50,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x15,
    .fault_bit_mask_num = IRLED_CURRENTS_FAULT_BIT_MASK_NUM, 
  }, 
  [CURRENTS_BAT_MOTOR] = 
  {
    .adc_type           = MICO_ADC_BAT_MOTOR_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = ADC_FACTOR_50,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x16,
    .fault_bit_mask_num = CHARGE_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  
  
#if 0
  [ADC_MULTI_CHANNEL] = 
  {
    .adc_type           = MICO_ADC_MULTI_CHANNAL,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'N',
    .convert_factor     = 50,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x17,
    .fault_bit_mask_num = BATIN_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
#endif
  
  
  [TEMP_24V_TS] = 
  {
    .adc_type           = MICO_ADC_24V_TS,
    .convert_type       = CONVERT_TYPE_TEMP,
    .isNeedDelay        = 'Y',
    .convert_factor     = 1,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x18,
    .fault_bit_mask_num = _24V_VOLTAGE_FAULT_BIT_MASK_NUM, 
  },
  [TEMP_12V_TS] = 
  {
    .adc_type           = MICO_ADC_12V_TS ,
    .convert_type       = CONVERT_TYPE_TEMP,
    .isNeedDelay        = 'Y',
    .convert_factor     = 1,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x19,
    .fault_bit_mask_num = _12V_VOLTAGE_FAULT_BIT_MASK_NUM, 
  },
  [TEMP_5V_TS] = 
  {
    .adc_type           = MICO_ADC_5V_TS,
    .convert_type       = CONVERT_TYPE_TEMP,
    .isNeedDelay        = 'Y',
    .convert_factor     = 1,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x1A,
    .fault_bit_mask_num = _5V_VOLTAGE_FAULT_BIT_MASK_NUM, 
  },
  [TEMP_AIR_TS] = 
  {
    .adc_type           = MICO_ADC_AIR_TS,
    .convert_type       = CONVERT_TYPE_TEMP,
    .isNeedDelay        = 'Y',
    .convert_factor     = 1,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x1B,
    .fault_bit_mask_num = _5V_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_24V_ALL] = 
  {
    .adc_type           = MICO_ADC_24V_ALL_C ,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'Y',
    .convert_factor     = ADC_FACTOR_5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x1C,
    .fault_bit_mask_num = _12V_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_12V_ALL] = 
  {
    .adc_type           = MICO_ADC_12V_ALL_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'Y',
    .convert_factor     = ADC_FACTOR_5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x1D,
    .fault_bit_mask_num = _24V_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
  [CURRENTS_5V_ALL] = 
  {
    .adc_type           = MICO_ADC_5V_ALL_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'Y',
    .convert_factor     = ADC_FACTOR_5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x1E,
    .fault_bit_mask_num = AMBIENT_TEMP_FAULT_BIT_BIT_MASK_NUM, 
  },
  [VOLTAGE_24V] = 
  {
    .adc_type           = MICO_ADC_24V_V,
    .convert_type       = CONVERT_TYPE_VOLTAGE,
    .isNeedDelay        = 'Y',
    .convert_factor     = 11,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x1F,
    .fault_bit_mask_num = _5V_REGULATOR_TEMP_FAULT_BIT_MASK_NUM, 
  },
  [VOLTAGE_12V] = 
  {
    .adc_type           = MICO_ADC_12V_V,
    .convert_type       = CONVERT_TYPE_VOLTAGE,
    .isNeedDelay        = 'Y',
    .convert_factor     = 21,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x20,
    .fault_bit_mask_num = _12V_REGULATOR_TEMP_FAULT_BIT_MASK_NUM, 
  },
  [VOLTAGE_5V] = 
  {
    .adc_type           = MICO_ADC_5V_V,
    .convert_type       = CONVERT_TYPE_VOLTAGE,
    .isNeedDelay        = 'Y',
    .convert_factor     = 2,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x21,
    .fault_bit_mask_num = RES_12V_CURRENTS_FAULT_BIT_MASK_NUM, 
  },  
  [VOLTAGE_BAT] = 
  {
    .adc_type           = MICO_ADC_BAT_V,
    .convert_type       = CONVERT_TYPE_VOLTAGE,
    .isNeedDelay        = 'Y',
    .convert_factor     = 21,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0,
    .fault_bit_mask_num = 0,
  },
  [CURRENTS_SENSOR_BOARD] = 
  {
    .adc_type           = MICO_ADC_5V_SENSOR_BOARD_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'Y',
    .convert_factor     = ADC_FACTOR_5,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0,
    .fault_bit_mask_num = 0,
  },
  [CURRENTS_12V_ROUTER] = 
  {
    .adc_type           = MICO_ADC_12V_ROUTER_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'Y',
    .convert_factor     = ADC_FACTOR_5,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0,
    .fault_bit_mask_num = 0,
  },
  
  
  
  
  [CURRENTS_24V_NV] = 
  {
    .adc_type           = MICO_ADC_NV_24V_C,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'Y',
    .convert_factor     = ADC_FACTOR_5,//5.95,//5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x02,
    .fault_bit_mask_num = RES_12V_CURRENTS_FAULT_BIT_MASK_NUM,
  },
  [CURRENTS_12V_NV] = 
  {
    .adc_type           = MICO_ADC_NV_12V_C    ,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'Y',
    .convert_factor     = ADC_FACTOR_5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x04,
    .fault_bit_mask_num = SYS_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
    [CURRENTS_KEYPAD] = 
  {
    .adc_type           = MICO_ADC_KEYPAD_C    ,
    .convert_type       = CONVERT_TYPE_CURRENTS,
    .isNeedDelay        = 'Y',
    .convert_factor     = ADC_FACTOR_5,
    .threshold_low      = 0,
    .threshold_high     = 0xFFFF,
    .err_duration_time  = 300/SYSTICK_PERIOD,
    .err_channel        = 0x04,
    .fault_bit_mask_num = SYS_CURRENTS_FAULT_BIT_MASK_NUM, 
  },
};

OSStatus VolDetect_Init( void )
{
  OSStatus err = kNoErr;  

  voltageConvert = &ramVoltageConvert;
  memset( voltageConvert, 0x0, sizeof(voltageData_t) );

#ifdef VOLTAGE_DEBUG
  tempMaxVoltageData = &ramTempMaxVoltageData;
  memset( tempMaxVoltageData, 0x0, sizeof(voltageData_t) );
  if( tempMaxVoltageData )
  {
    tempMaxVoltageData->bat_voltage = 10000;//set temp max voltage is 100v
  }
#endif
  voltageConvertData = (voltageConvertData_t *)malloc( sizeof(voltageConvertData_t) );
  require_action( voltageConvertData, exit, err = kNoMemoryErr );
  memset( voltageConvertData, 0x0, sizeof(voltageConvertData_t) );
  if( voltageConvertData )
  {
  }
  analog_multiplexer_init();
  err = multi_channel_adc_dma_init();
  require_noerr_quiet( err, exit );
exit:
  if( kNoErr != err )
  {
    vol_detect_log("voltage detection init error!");
  }
  else
    vol_detect_log("voltage detection init success!");
  return err;
}

static uint16_t processChannelsData( adc_channel_t type )
{
  uint16_t      readData;
  
  struct convert_adc_data *pConvertAdcData = (struct convert_adc_data *)&convert_data[type];
  
  if( pConvertAdcData->isNeedDelay == 'Y' )
  {
    select_multi_channel( pConvertAdcData->adc_type );
    HAL_Delay(1);//10ms
    //delay_us(8000);
  }
  
  readData = (uint16_t)(read_channel_values_mV( pConvertAdcData->adc_type ) * pConvertAdcData->convert_factor );
  
  if( readData > pConvertAdcData->threshold_high || readData < pConvertAdcData->threshold_low )
  {
     if( !pConvertAdcData->err_start_time )
     {
       pConvertAdcData->err_start_time = (uint16_t)os_get_time();
     }
     else
     {
       if( (uint16_t)os_get_time() - pConvertAdcData->err_start_time > \
         pConvertAdcData->err_duration_time )
       {
         *(uint32_t *)voltageConvertData->faultBitTemp |= \
           (uint32_t)(0x1) << pConvertAdcData->fault_bit_mask_num;
         pConvertAdcData->err_value = readData;
         boardStatus->errChannel = pConvertAdcData->err_channel;
         boardStatus->errValue = readData;
       }
     }
  }
  else
  {
    if( pConvertAdcData->err_start_time != 0 )
    {
       pConvertAdcData->err_start_time = 0;
    }
  }
  return readData;
}

static uint8_t  sample_index = TEMP_24V_TS;
static void computeVoltage( void )
{  
  //boardStatus->vBatLevel = get_percentage_from_battery_voltage( voltageConvert->bat_voltage );
  
  voltageConvert->_5V_reserve1_currents = processChannelsData( CURRENTS_5V_RES );
  //voltageConvert->_24V_nv_currents = processChannelsData( CURRENTS_24V_NV );  
  //voltageConvert->_12V_nv_currents = processChannelsData( CURRENTS_12V_NV );
  voltageConvert->_48V_extend_currents = processChannelsData( CURRENTS_48V_EXTEND );
  
  voltageConvert->_12V_extend_currents = processChannelsData( CURRENTS_12V_EXTEND );
  voltageConvert->motor_currents = processChannelsData( CURRENTS_5V_MOTOR );
  voltageConvert->slam_currents = processChannelsData( CURRENTS_24V_SLAM );
  voltageConvert->_2_1_pa_currents = processChannelsData( CURRENTS_12V_2_1_PA );
  
  voltageConvert->pad_currents = processChannelsData( CURRENTS_12V_PAD );
  voltageConvert->printer_currents = processChannelsData( CURRENTS_24V_PRINTER );
  voltageConvert->x86_currents = processChannelsData( CURRENTS_12V_X86 );
  voltageConvert->ir_led_currents = processChannelsData( CURRENTS_IRLED );
  
  voltageConvert->_5V_leds_currents = processChannelsData( CURRENTS_5V_LEDS );
  
  voltageConvert->recharge_currents = processChannelsData( CURRENTS_RECHARGE );
  voltageConvert->_24V_extend_currents = processChannelsData( CURRENTS_24V_EXTEND );    
  voltageConvert->charge_currents = processChannelsData( CURRENTS_CHARGE );
  voltageConvert->batin_currents = processChannelsData( CURRENTS_BATIN );
  
  voltageConvert->vbus_currents = processChannelsData( CURRENTS_VBUS );
  voltageConvert->bat_motor_currents = processChannelsData( CURRENTS_BAT_MOTOR );

  switch( sample_index )
  {
  case TEMP_24V_TS:
    voltageConvert->_24V_temp = get_ntc_temp_from_voltage(processChannelsData( TEMP_24V_TS ));
    sample_index = TEMP_12V_TS;
  break;
  case TEMP_12V_TS:
    voltageConvert->_12V_temp = get_ntc_temp_from_voltage(processChannelsData( TEMP_12V_TS ));
    sample_index = TEMP_5V_TS;
  break;
  case TEMP_5V_TS:
    voltageConvert->_5V_temp = get_ntc_temp_from_voltage(processChannelsData( TEMP_5V_TS ));
    sample_index = TEMP_AIR_TS;
  break;
  case TEMP_AIR_TS:
    voltageConvert->air_temp = get_ntc_temp_from_voltage(processChannelsData( TEMP_AIR_TS ));
    sample_index = CURRENTS_24V_ALL;
  break;
  case CURRENTS_24V_ALL:
    voltageConvert->_24V_all_currents = processChannelsData( CURRENTS_24V_ALL );
    sample_index = CURRENTS_12V_ALL;
  break;
  case CURRENTS_12V_ALL:
    voltageConvert->_12V_all_currents = processChannelsData( CURRENTS_12V_ALL );
    sample_index = CURRENTS_5V_ALL;
  break;
  case CURRENTS_5V_ALL:
    voltageConvert->_5V_all_currents = processChannelsData( CURRENTS_5V_ALL );
    sample_index = VOLTAGE_24V;
  break;
  case VOLTAGE_24V:
    voltageConvert->_24V_voltage = processChannelsData( VOLTAGE_24V ) ;
    sample_index = VOLTAGE_12V;
  break;
  case VOLTAGE_12V:
    voltageConvert->_12V_voltage = processChannelsData( VOLTAGE_12V ) ;
    sample_index = VOLTAGE_5V;
  break;
  case VOLTAGE_5V:
    voltageConvert->_5V_voltage = processChannelsData( VOLTAGE_5V );
    sample_index = VOLTAGE_BAT;
  break;
  case VOLTAGE_BAT:
    voltageConvert->bat_voltage = processChannelsData( VOLTAGE_BAT );
    sample_index = CURRENTS_SENSOR_BOARD;
  break;
  case CURRENTS_SENSOR_BOARD:
    voltageConvert->sensor_board_currents = processChannelsData( CURRENTS_SENSOR_BOARD );
    sample_index = CURRENTS_12V_ROUTER;
  break;
  case CURRENTS_12V_ROUTER:
    voltageConvert->_12V_router_currents = processChannelsData( CURRENTS_12V_ROUTER );
    sample_index = CURRENTS_24V_NV;
  break;
  
  
  
  
  case CURRENTS_24V_NV:
    voltageConvert->_24V_nv_currents = processChannelsData( CURRENTS_24V_NV );
    sample_index = CURRENTS_12V_NV;
  break;
  case CURRENTS_12V_NV:
    voltageConvert->_12V_nv_currents = processChannelsData( CURRENTS_12V_NV );
    sample_index = CURRENTS_KEYPAD;
  break;
  case CURRENTS_KEYPAD:
    voltageConvert->keypad_currents = processChannelsData( CURRENTS_KEYPAD );
    sample_index = TEMP_24V_TS;
  break;
  }
}

void PrintAdcData(void)
{
    printf("\r\n");
    //printf("battery percertage: %d%\r\n", boardStatus->vBatLevel);//get_percentage_from_battery_voltage( voltageConvert->bat_voltage ) );
    for( uint8_t i = 1; i <= sizeof(voltageData_t)/2; i++ )
    {
        if( i % 10 == 0)
        {
            printf("\r\n");
        }   
        printf( "%d: %d\t", i, *((uint16_t *)voltageConvert + i - 1) );
    }     
    printf("\r\n");
}

#define LOW_POWER_WARNING_DEBAUNCE_TIME     15*1000/SYSTICK_PERIOD
#define LOW_POWER_POEWR_OFF_DEBAUNCE_TIME   15*1000/SYSTICK_PERIOD
#define NORMAL_POWER_DEBAUNCE_TIME          15*1000/SYSTICK_PERIOD
static uint32_t low_power_warning_start_time = 0;
static uint32_t low_power_power_off_start_time = 0;
static uint32_t normal_power_start_time = 0;
//static uint32_t  batteryPercentageStartTime = 0;
void VolDetect_Tick( void )
{
    uint8_t percentage = 0;
    computeVoltage();
#if 0
    if( os_get_time() - batteryPercentageStartTime >= 1000/SYSTICK_PERIOD )
    {
        batteryPercentageStartTime = os_get_time();
        battery_percentage_1s_period();
    }
#endif
    if( ( YES == voltageDebug.isNeedUpload ) && IS_SEND_RATE_DATA( voltageDebug.uploadRate ) )
    {     
        if( voltageDebug.uploadRate == SEND_RATE_SINGLE )
        {
            //        uploadCurrentInformation( serial, voltageConvert );
            voltageDebug.isNeedUpload = NO;
            boardStatus->sysStatus &= ~(uint16_t)STATE_IS_AUTO_UPLOAD;
        }
        else
        {
            if( (os_get_time() - voltageDebug.uploadFlagTime) >= sendRateToTime(voltageDebug.uploadRate) )
            {
                boardStatus->sysStatus |= (uint16_t)STATE_IS_AUTO_UPLOAD;
                voltageDebug.uploadFlagTime = os_get_time();
                uploadCurrentInformation( serial, voltageConvert );
            }
        }
    }
    if( PRINT_ONCE == voltageDebug.printType || PRINT_PEROID == voltageDebug.printType )
    {
        if( voltageDebug.printType == PRINT_ONCE )
        {
            voltageDebug.printType = PRINT_NO;
        }
        if( PRINT_PEROID == voltageDebug.printType )
        {
            if( os_get_time() - voltageDebug.startTime < voltageDebug.peroid/SYSTICK_PERIOD )
            {
                return;
            }
        }     
        PrintAdcData();


        if( PRINT_PEROID == voltageDebug.printType )
        {
            voltageDebug.startTime = os_get_time();
            //vol_detect_log("print peroid = %d ms",voltageDebug.peroid );
        }
    }
#ifdef  VOLTAGE_DEBUG
    if( PRINT_ONCE == voltageDebug.printMaxType || RESET_MAX_BUF == voltageDebug.printMaxType )
    {
        if( voltageDebug.printMaxType == PRINT_ONCE )
        {
            voltageDebug.printMaxType = PRINT_NO;
        }
        if( RESET_MAX_BUF == voltageDebug.printMaxType )
        {
            memset(tempMaxVoltageData, 0x0, sizeof(voltageData_t));
            if( tempMaxVoltageData )
            {
                tempMaxVoltageData->bat_voltage = 10000;//set temp max voltage is 100v
            }
            voltageDebug.printMaxType = PRINT_NO;
        }
    }
#endif //#ifdef  VOLTAGE_DEBUG
    if( SWITCH_ON == switch_user->switchOnOff )
    {
        
#if 1
        if(battery_pack.com_status == true )
        {
            percentage = battery_pack.percentage;
            if( (percentage <= VBAT_POWER_LOW_WARNING_PERCENTAGE) && percentage > VBAT_POWER_OFF_PERCENTAGE )
            {
                if( low_power_warning_start_time == 0)
                {
                    low_power_warning_start_time = os_get_time();
                }
                if( os_get_time() - low_power_warning_start_time >= LOW_POWER_WARNING_DEBAUNCE_TIME )
                {
                    vol_detect_log(" Low power ! ! \r\n Power is %d%", percentage);
                    //SetSerialLedsEffect( LIGHTS_MODE_LOW_POWER, NULL, 0 );
                    
                    low_power_warning_start_time = 0;
                    boardStatus->sysStatus |= STATE_IS_LOW_POWER;
                }
                
            }
            else
            {
                if( low_power_warning_start_time != 0)
                {
                    low_power_warning_start_time = 0;
                }
            }
            
            
            if(percentage > VBAT_POWER_LOW_WARNING_PERCENTAGE)
            {   
                if(normal_power_start_time == 0)
                {
                    normal_power_start_time = os_get_time();
                }
                if(os_get_time() - normal_power_start_time > NORMAL_POWER_DEBAUNCE_TIME)
                {
                    boardStatus->sysStatus &= ~STATE_IS_LOW_POWER;
                    //SetSerialLedsEffect( LIGHTS_MODE_NOMAL, NULL, 0 );
                    normal_power_start_time = os_get_time();
                }
                
            }
            else
            {
                if( normal_power_start_time != 0)
                {
                    normal_power_start_time = 0;
                }
            }
            
            if( percentage <= VBAT_POWER_OFF_PERCENTAGE )
            {
                if( low_power_power_off_start_time == 0)
                {
                    low_power_power_off_start_time = os_get_time();
                }
                if( os_get_time() - low_power_power_off_start_time >= LOW_POWER_POEWR_OFF_DEBAUNCE_TIME )
                {
                    vol_detect_log(" Going to power off ! ! \r\n Power is %d%", percentage);
                    
                    PowerOffDevices();
                    low_power_power_off_start_time = 0;
                }
            }
            else
            {
                if( low_power_power_off_start_time != 0 )
                {
                    low_power_power_off_start_time = 0;
                }
            }
        }

#endif
    }
}

#if 0
//1s compute once
#define BUFFER_SIZE   20
static uint16_t percentage[BUFFER_SIZE];
static uint32_t percentage_sum;
void battery_percentage_1s_period( void )
{
    uint16_t batVolPlusInsideRes;
    uint16_t minBatLevel = 0;

    for( uint32_t i = 0; i < BUFFER_SIZE - 1; i++ )
    {
      percentage[i] = percentage[i+1];
    }
#ifdef NOT_USE_TMP
    batVolPlusInsideRes = (uint16_t)(voltageConvert->bat_voltage + \
      voltageConvert->sys_all_currents * BAT_RES);//V(bat)=V(output)+I(bat)*R(batRes)
    percentage[BUFFER_SIZE-1] = get_percentage_from_battery_voltage( batVolPlusInsideRes );
#endif
    percentage_sum = 0;
    uint8_t percentage_sum_count = 0;
    for( uint32_t i = 0; i < BUFFER_SIZE; i++ )
    {
      if( percentage[i] != 0 )
      {
        percentage_sum += percentage[i];
        percentage_sum_count ++;
      }
    }
    minBatLevel = (uint16_t)( percentage_sum/(percentage_sum_count * 1.0) );
    
    if(!(boardStatus->sysStatus & (uint16_t)STATE_IS_CHARGING))
    {
        if(minBatLevel < boardStatus->vBatLevel)
        {
            boardStatus->vBatLevel = minBatLevel;
        }
    }
    else
    {
        boardStatus->vBatLevel = minBatLevel;
    }
    //vol_detect_log("vBatLevel is %d", boardStatus->vBatLevel);
}
#endif

/*********************END OF FILE**************/
