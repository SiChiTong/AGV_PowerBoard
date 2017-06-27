/**
 ******************************************************************************
 * @file    multi_channel_detection.c
 * @author  Adam Huang
 * @version V1.0.0
 * @date    29-Nov-2016
 * @brief   
 ******************************************************************************
*/

#include "multi_channel_detection.h"

#define   ADC1_DMA_CHANNELS_NUM  (14)
#define   FILTER_TIMES          (5)
#define   ADC1_DMA_BURRER_SIZE   (ADC1_DMA_CHANNELS_NUM*FILTER_TIMES)

#define   ADC3_DMA_CHANNELS_NUM  (6)
#define   FILTER_TIMES          (5)
#define   ADC3_DMA_BURRER_SIZE   (ADC3_DMA_CHANNELS_NUM*FILTER_TIMES)

extern const platform_adc_t             platform_adc_peripherals[];
__IO uint16_t adc1_dma_buffer[ADC1_DMA_BURRER_SIZE];
__IO uint16_t adc3_dma_buffer[ADC3_DMA_BURRER_SIZE];

void analog_multiplexer_init( void )
{
  platform_pin_config_t pin_config;
  
  pin_config.gpio_speed = GPIO_SPEED_MEDIUM;
  pin_config.gpio_mode = GPIO_MODE_OUTPUT_PP;
  pin_config.gpio_pull = GPIO_PULLUP;
  
  MicoGpioInitialize( (mico_gpio_t)MICO_GPIO_SWITCH_EN, &pin_config );
  MicoGpioInitialize( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0, &pin_config );
  MicoGpioInitialize( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1, &pin_config );
  MicoGpioInitialize( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2, &pin_config );
  MicoGpioInitialize( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3, &pin_config );

  MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_EN );
  MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
  MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
  MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
  MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
}


OSStatus select_multi_channel( mico_adc_t adc )
{
  OSStatus err = kNoErr;
  if ( adc >= MICO_ADC_MAX || adc <  MICO_ADC_24V_TS )
    return kUnsupportedErr;
  
  MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_EN );
  switch( adc )
  {
  case MICO_ADC_24V_TS:
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break;
  case MICO_ADC_12V_TS:
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break;
  case MICO_ADC_5V_TS:
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break;
  case MICO_ADC_AIR_TS:
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break;
  case MICO_ADC_24V_ALL_C:
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break;
  case MICO_ADC_12V_ALL_C:
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break;
  case MICO_ADC_5V_ALL_C:
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break;
  case MICO_ADC_24V_V:
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break; 
  case MICO_ADC_12V_V:
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break;
  case MICO_ADC_5V_V:
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break;
  case MICO_ADC_BAT_V:
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break;
  case MICO_ADC_5V_SENSOR_BOARD_C:
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break;
  case MICO_ADC_5V_ROUTER_C:
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL0 );
    MicoGpioOutputLow( (mico_gpio_t)MICO_GPIO_SWITCH_SEL1 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL2 );
    MicoGpioOutputHigh( (mico_gpio_t)MICO_GPIO_SWITCH_SEL3 );
    break;
  default:
    break;
  } 
  return err;
}

#define ADC1_USED_CHANNELELS    (14)
#define ADC3_USED_CHANNELELS    (6)
#define SAMPLE_CYCLE            (235)

OSStatus multi_channel_adc_dma_init( void )
{
  OSStatus err = kNoErr;
  
  err = MicoAdcStreamInitializeEarly( MICO_ADC_5V_RESERVE_C, ADC1_USED_CHANNELELS );
  require_noerr_quiet( err, exit );
  
  err = MicoAdcStreamAddChannel( MICO_ADC_5V_RESERVE_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  
  err = MicoAdcStreamAddChannel( MICO_ADC_24V_NV_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_12V_NV_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_48V_EXTEND_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_12V_EXTEND_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_MOTOR_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_SLAM_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_2_1_PA_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_PAD_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_PRINTER_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_X86_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_IRLED_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_5V_LEDS_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );

  select_multi_channel( MICO_ADC_24V_TS );
  err = MicoAdcStreamInitializeLate( MICO_ADC_5V_LEDS_C, (void *)adc1_dma_buffer, ADC1_DMA_BURRER_SIZE );
  require_noerr_quiet( err, exit );
/*----------------------------------------------------------------------------*/  
  err = MicoAdcStreamInitializeEarly( MICO_ADC_24V_EXTEND_C, ADC3_USED_CHANNELELS );
  require_noerr_quiet( err, exit );
  
  err = MicoAdcStreamAddChannel( MICO_ADC_24V_EXTEND_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_24V_EXTEND_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_CHARGE_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_BATIN_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  err = MicoAdcStreamAddChannel( MICO_ADC_VBUS_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
   err = MicoAdcStreamAddChannel( MICO_ADC_BAT_MOTOR_C, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
   err = MicoAdcStreamAddChannel( MICO_ADC_MULTI_CHANNAL, SAMPLE_CYCLE );
  require_noerr_quiet( err, exit );
  
  err = MicoAdcStreamInitializeLate( MICO_ADC_MULTI_CHANNAL, (void *)adc3_dma_buffer, ADC3_DMA_BURRER_SIZE );
  require_noerr_quiet( err, exit );
exit:
  return err;
}

uint16_t  read_channel_values_mV( mico_adc_t adc )
{
  uint32_t      temp_total = 0;
  
  if( adc >= MICO_ADC_MAX )
    return 0;
  if ( /*adc >= MICO_ADC_5V_RES1 && */adc <= MICO_ADC_5V_LEDS_C )
  {
    for( uint8_t i = 0; i < FILTER_TIMES; i++ )
    {
      temp_total += (uint16_t)adc1_dma_buffer[platform_adc_peripherals[adc].rank - 1 + i * ADC1_DMA_CHANNELS_NUM ];
    }
    temp_total /= FILTER_TIMES;
    return (uint16_t)(temp_total/4096.0*3.3*1000);
  }
  else if ( adc > MICO_ADC_MULTI_CHANNAL && adc <=  MICO_ADC_5V_ROUTER_C )
  {    
    for( uint8_t i = 0; i < FILTER_TIMES; i++ )
    {
      temp_total += (uint16_t)adc3_dma_buffer[ADC3_USED_CHANNELELS - 1 + i * ADC3_DMA_CHANNELS_NUM ];
    }
    temp_total /= FILTER_TIMES;
    return (uint16_t)(temp_total/4096.0*3.3*1000);
  }
  else
  {
    for( uint8_t i = 0; i < FILTER_TIMES; i++ )
    {
      temp_total += (uint16_t)adc3_dma_buffer[platform_adc_peripherals[adc].rank - 1 + i * ADC3_DMA_CHANNELS_NUM ];
    }
    temp_total /= FILTER_TIMES;
    return (uint16_t)(temp_total/4096.0*3.3*1000);
  }
}

#define FIXIT_FACTOR    (5)
#define FIXIT_FACTOR_VOL    (1)
#define FIXIT_FACTOR_CUR     (50)
void read_multi_convert_values( void )
{
#ifdef NOT_USE_TMP  
  printf("\r\n");
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_5V_RES1 )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_12V_RES2 )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_BAT_NV )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_12V_NV )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_ROUTER_12V )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_DYP )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_SENSOR )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_DLP )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_MOTOR )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_24V_RES1 )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_2_1_PA )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_PAD )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_PRINTER )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_X86 )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_IRLED )* FIXIT_FACTOR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_LEDS )* FIXIT_FACTOR));
#if 1
  printf("follows are adc3 channels\r\n");
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_CHARGE )* FIXIT_FACTOR_CUR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_BATIN )* FIXIT_FACTOR_CUR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_VBUS )* FIXIT_FACTOR_CUR));
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_BAT_MOTOR )* FIXIT_FACTOR_CUR));
  select_multi_channel( MICO_ADC_24V_TS );
  HAL_Delay(1);
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_24V_TS )* FIXIT_FACTOR_VOL));
  select_multi_channel( MICO_ADC_12V_TS );
  HAL_Delay(1);
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_12V_TS )* FIXIT_FACTOR_VOL));
  select_multi_channel( MICO_ADC_5V_TS );
  HAL_Delay(1);
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_5V_TS )* FIXIT_FACTOR_VOL));
  select_multi_channel( MICO_ADC_AIR_TS );
  HAL_Delay(1);
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_AIR_TS )* FIXIT_FACTOR_VOL));
  select_multi_channel( MICO_ADC_24V_ALL );
  HAL_Delay(1);
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_24V_ALL )* FIXIT_FACTOR));
  select_multi_channel( MICO_ADC_12V_ALL );
  HAL_Delay(1);
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_12V_ALL )* FIXIT_FACTOR));
  select_multi_channel( MICO_ADC_5V_ALL );
  HAL_Delay(1);
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_5V_ALL )* FIXIT_FACTOR));
  select_multi_channel( MICO_ADC_VDET_24V );
  HAL_Delay(1);
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_VDET_24V )* 11));
  select_multi_channel( MICO_ADC_VDET_12V );
  HAL_Delay(1);
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_VDET_12V )* 11));
  select_multi_channel( MICO_ADC_VDET_5V );
  HAL_Delay(1);
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_VDET_5V )* 2));
  select_multi_channel( MICO_ADC_VDET_BAT );
  HAL_Delay(1);
  printf("%d\t",(uint16_t)(read_channel_values_mV( MICO_ADC_VDET_BAT )* 11));
#endif
#endif
}
