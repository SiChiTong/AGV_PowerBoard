

#include "main.h"
#include "stm32f1xx_hal.h"


#include "mico.h"
#include "platform.h"
#include "platform_internal.h"
#include "platform_config.h"
#include "board_init.h"
#include "multi_channel_detection.h"
#include "serial_leds.h"
#include "voltage_detect.h"
#include "protocol.h"
#include "app_platform.h"
#include "upgrade_flash.h"
#include "can_protocol.h"

#include "battery.h"
#include "fifo.h"
#include "fan.h"

#define os_PowerBoard_log(format, ...)  custom_log("PowerBoard", format, ##__VA_ARGS__)

extern void Main_Menu(void);
#define Application_REVISION "v2.1"

#ifdef SIZE_OPTIMIZE
const t char menu[] =
"\r\n"
"PowerBoard application for %s, %s, HARDWARE_REVISION: %s\r\n"
"0:BOOTUPDATE,"
"1:FWUPDATE,"
"2:DRIVERUPDAT,"
"3:PARAUPDATE,"
"4:FLASHUPDATE,"
"5:MEMORYMAP,"
"6:BOOT,"
"7:REBOOT";
#else
const char menu[] =
"\r\n"
"PowerBoard application for %s, %s, HARDWARE_REVISION: %s\r\n"
"+ command -------------------------+ function ------------+\r\n"
"| 0:BOOTUPDATE    <-r>             | Update bootloader    |\r\n"
"| 1:FWUPDATE      <-r>             | Update application   |\r\n"
"| 2:PARUPDATE     <-id n><-r><-e>  | Update MICO partition|\r\n"
"| 3:FLASHUPDATE   <-dev device>    |                      |\r\n"
"|  <-e><-r><-start addr><-end addr>| Update flash content |\r\n"
"| 4:MEMORYMAP                      | List flash memory map|\r\n"
"| 5:BOOT                           | Excute application   |\r\n"
"| 6:REBOOT                         | Reboot               |\r\n"
"| 8:GPIO 9:IRLed A:Stop vol print B:Start vol print       |\r\n"
"| C:Log Off D:Log On E:Print charger plugin times ?:help  |\r\n"
"+----------------------------------+----------------------+\r\n"
"|    (C) COPYRIGHT 2016 MUYE Corporation  By Driver Group |\r\n"
" Notes:\r\n"
" -e Erase only  -r Read from flash -dev flash device number\r\n"
"  -start flash start address -end flash start address\r\n"
" Example: Input \"4 -dev 0 -start 0x400 -end 0x800\": Update \r\n"
"          flash device 0 from 0x400 to 0x800\r\n";
#endif
int8_t isNeedAutoBoot( void );
#ifdef MIKE_TEST
void test_power_tick( void );
#endif
void OneWireLedTest(void);

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
uint8_t test_data[] = {0x5a,8,1,1,1,1,0x66,0xa5};
uint8_t rcv_buf[50];
HAL_StatusTypeDef uart_err;
 

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

int main(void)
{
    HAL_Init();

    init_clocks();
    bsp_Init(); 
    init_architecture();
    init_platform();
    printf ( menu, MODEL, SW_VERSION, HARDWARE_REVISION );

    os_PowerBoard_log( "System clock = %d Hz",HAL_RCC_GetHCLKFreq() );

    Platform_Init();
    
    VolDetect_Init();
    Protocol_Init();
    startTps611xx();
    FanInit();
    
    battery_init();
    
    FifoInit(fifo, fifo_data_in_ram, RCV_DATA_LEN_MAX);
      
    MicoCanInitialize( MICO_CAN1 );
    

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    
    if(HAL_UART_Receive_IT(&huart2,rcv_buf,1)!=HAL_OK)Error_Handler();
    
    if( !isNeedAutoBoot() )
    {   
        while(os_get_time() <= flashTable.AutoBootDelayTime*1000/SYSTICK_PERIOD);
        PowerOnDevices();
    } 
    
    for(;;)
    {
        Platform_Tick();
        //protocol_period(); 
        VolDetect_Tick();
        can_protocol_period();
        Main_Menu();
        battery_period(); 
        leds_protocol_period();
    }

}


color_t color_test[] = 
{
    [0]     = {0xc8, 0x32, 0x00 },  
    [1]     = {0x96, 0x19, 0x00 },  
    [2]     = {0x82, 0x1e, 0x00 },
    [3]     = {0xb4, 0x1e, 0x00 },  
    [4]     = {0xb4, 0x14, 0x00 },
    [5]     = {255, 165, 0 },
};

void OneWireLedTest(void)
{
#define ONE_WIRE_LED_TEST_PERIOD    2500/SYSTICK_PERIOD//10s

    static uint32_t start_time = 0;
    static uint8_t new_tick = 0;
    static uint8_t last_tick = 1;
    static uint8_t init_flag = 0;
    if(init_flag == 0)
    {
        init_flag = 1;
       
    }
    
    
    if(os_get_time() - start_time >= ONE_WIRE_LED_TEST_PERIOD)
    {
        start_time = os_get_time();
        new_tick++;
        
    }
    
    if(new_tick >= sizeof(color_test)/sizeof(color_test[0]))
    {
        new_tick = 0;
         
    }
    //else
    {
        if(new_tick != last_tick)
        {
            SetSerialLedsEffect( (light_mode_t)new_tick, NULL, 50 );
            //SetSerialLedsEffect(LIGHTS_MODE_SETTING, &color_test[new_tick], 200);
            //printf("hello hello");
        }    
    }
    last_tick = new_tick;
}

int8_t isNeedAutoBoot( void )
{
  OSStatus err = kNoErr;
  uint32_t flash_table_offset = 0x0;
  save_flash_data_t        flashTableTmp;
  err = MicoFlashRead( MICO_PARTITION_PARAMETER_2, &flash_table_offset, (uint8_t *)&flashTableTmp, sizeof(save_flash_data_t) );
  if( err )
  {
    printf("\r\n read flash data err\r\n");
    return -1;
  }
  if( flashTableTmp.isNeedAutoBoot == 'Y' )
  {//
    flashTableTmp.isNeedAutoBoot = 'N';
    flashTable.AutoBootDelayTime = flashTableTmp.AutoBootDelayTime;
    MICOBootConfiguration( &flashTableTmp );
    return 0;
  }
  return -1;
}


/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_NVIC_SetPriority(USART2_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  /*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
#if 0
  __HAL_RCC_GPIOE_CLK_ENABLE();
  
  /*Configure GPIO pin : PE10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET);
  
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  /*Configure GPIO pin : PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/





#if 1
static uint32_t    indicatorFreshCount = 0;
void SysLed(void)
{
    indicatorFreshCount ++;
    if( indicatorFreshCount > 50 )
    {
        indicatorFreshCount = 0;
        MicoGpioOutputTrigger( MICO_GPIO_SYS_LED );
    } 
}
#endif
