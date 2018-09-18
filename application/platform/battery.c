/* 
*  Author: Adam Huang
*  Date:2017/6/22
*/
#include "mico.h"
#include "app_platform.h"
#include "battery.h"
#include "StringUtils.h"
#include "serial_uart.h"

#define battery_log(M, ...) custom_log("Battery", M, ##__VA_ARGS__)
#define battery_log_trace() custom_log_trace("Battery")

#ifndef BATT_BUFFER_SIZE
#define BATT_BUFFER_SIZE   64
#endif

extern platform_uart_t platform_uart_peripherals[];
extern platform_uart_driver_t platform_uart_drivers[];

static const platform_uart_config_t batt_uart_config =
{
  .baud_rate    = BATT_UART_BAUDRATE,
  .data_width   = DATA_WIDTH_8BIT,
  .parity       = NO_PARITY,
  .stop_bits    = STOP_BITS_1,
  .flow_control = FLOW_CONTROL_DISABLED,
  .flags        = 0,
};

//static volatile ring_buffer_t       batt_rx_buffer;
//static volatile uint8_t             batt_rx_data[BATT_BUFFER_SIZE];
//mico_mutex_t        batt_rx_mutex;
//mico_mutex_t        batt_tx_mutex;

#define RX_MAX_DATA_LENGTH      512
#define TX_MAX_DATA_LENGTH      100
static volatile uint8_t rx_data[RX_MAX_DATA_LENGTH];
static volatile uint8_t tx_data[TX_MAX_DATA_LENGTH];

#define READ_INFO_LENGTH        10
static uint8_t read_info_buff[READ_INFO_LENGTH];

typedef struct {
  volatile uint8_t          isDetectedSOI;
  volatile uint8_t          isDetectedEOI;
  uint8_t                   cmd_type;
  uint8_t*                  pRxData;
  uint8_t*                  pTxData;
  const uint8_t*            rx_data_buff;
  const uint8_t*            tx_data_buff;
} battery_data_t;

static battery_data_t       battery_data;
battery_data_t*             pBatteryData = &battery_data;

static void irq_clear_err(UART_HandleTypeDef *huart);
OSStatus check_and_send(uint8_t *buff, uint32_t length);
void battery_activate( void );



OSStatus battery_init( void )
{
  OSStatus err = kNoErr;

  err = MicoUartInitialize( BATT_UART, &batt_uart_config, NULL );
  irq_clear_err( platform_uart_drivers[BATT_UART].uart_handle );
  __HAL_UART_CLEAR_FLAG( platform_uart_drivers[BATT_UART].uart_handle, UART_IT_RXNE );
  __HAL_UART_ENABLE_IT( platform_uart_drivers[BATT_UART].uart_handle, UART_IT_RXNE );

  //CLEAR_BIT( platform_uart_drivers[BATT_UART].uart_handle->Instance->CR1, UART_MODE_RX );
  
  //StartDmaReciveEx( BATT_UART, (uint8_t *)rx_data, RX_MAX_DATA_LENGTH );
  
  
 // battery_log( "init err = %d", err );
  battery_protocol_init();
  return err;
}

void battery_protocol_init( void )
{
  if( !pBatteryData )
  {
    pBatteryData = &battery_data;
  }
  else
  {
    pBatteryData->rx_data_buff = (uint8_t const *)rx_data;
    pBatteryData->pRxData = (uint8_t *)pBatteryData->rx_data_buff;
    pBatteryData->tx_data_buff = (uint8_t const *)tx_data;
    pBatteryData->pTxData = (uint8_t *)pBatteryData->tx_data_buff;
    
    pBatteryData->isDetectedSOI = 0;
    pBatteryData->isDetectedEOI = 0;
  }
  battery_activate();
}

typedef union {
  struct {    
    uint16_t    lenid       : 12;
    uint16_t    lchecksum   : 4;
  } length_struct;
  uint16_t length_data;
} length_data_union;

uint16_t getLengthDataFromLength( uint16_t data_length )
{
  length_data_union length_data_u;
  uint8_t length_check = 0;
  uint16_t length = data_length;
  
  if( length > 0xFFF )
  {
    return 0;
  }
  
  length_check += (uint8_t)length;
  length_check += (uint8_t)(length >> 4);
  length_check += (uint8_t)(length >> 8);
  length_check = ~(uint8_t)length_check;
  length_check += 1;
  
  length_data_u.length_struct.lchecksum = length_check;
  length_data_u.length_struct.lenid = length;
  
  return length_data_u.length_data;
}

uint16_t getLengthFromLengthData( uint16_t length_data )
{
  length_data_union length_data_u;
  uint8_t length_check = 0;
  uint16_t length = 0;
  
  length_data_u.length_data = length_data;
  length = length_data_u.length_struct.lenid;
  
  length_check += (uint8_t)length;
  length_check += (uint8_t)(length >> 4);
  length_check += (uint8_t)(length >> 8);
  
  length_check = ~(uint8_t)length_check;
  length_check += 1;
  length_check %= 16;
  
  if( length_check != length_data_u.length_struct.lchecksum )
  {
    battery_log("length check isn't right");
  }
  
  return length;
}

OSStatus battery_common_read( uint8_t cid2, uint8_t* info, uint16_t info_len )
{
  OSStatus err = kNoErr;
  battery_protocol_t read_data;
  char *convert_str;
  //static uint8_t test = 0x10;
  require_action_quiet( ( info != NULL ) && ( info_len <= READ_INFO_LENGTH ), exit, err = kParamErr);
  
  read_data.soi = 0x7E;//'~'
  read_data.ver = 0x20;//test++;//
  read_data.adr = 0x01;
  read_data.cid1 = 0x46;
  read_data.cid2 = cid2;
  WriteBig16( &read_data.length_h, getLengthDataFromLength(info_len) );
  
  convert_str = DataToHexString((uint8_t *)&read_data.ver, 6);  
  memcpy( (uint8_t *)pBatteryData->tx_data_buff + 1, convert_str, 2*6 );
  free(convert_str);
  
  read_data.info = info;
  memcpy( (uint8_t *)pBatteryData->tx_data_buff + 13, read_data.info, info_len );
  err = check_and_send( (uint8_t *)pBatteryData->tx_data_buff, 13 + info_len );
  
exit:
  return err;
}

void battery_activate( void )
{
  *read_info_buff = '8';
  *(read_info_buff + 1) = '5';
  *(read_info_buff + 2) = '2';
  *(read_info_buff + 3) = '0';
  pBatteryData->cmd_type = 0x82;
  battery_common_read( 0x82, read_info_buff, 4);
}

void battery_read_infomation( void )
{
  *read_info_buff = '0';
  *(read_info_buff + 1) = '1';
  pBatteryData->cmd_type = 0x42;
  battery_common_read( 0x42, read_info_buff, 2);
}

void battery_read_warm_info( void )
{
  *read_info_buff = '0';
  *(read_info_buff + 1) = '1';
  pBatteryData->cmd_type = 0x44;
  battery_common_read( 0x44, read_info_buff, 2);
}

void battery_get_system_param( void )
{
  pBatteryData->cmd_type = 0x47;
  battery_common_read( 0x47, read_info_buff, 0);
}

OSStatus check_and_send(uint8_t *buff, uint32_t length)
{
  OSStatus err = kNoErr;
  uint32_t check_num = length - 1;
  uint16_t check_sum = 0;
  uint16_t check_sum_big = 0;
  char *convert_str;
  
  require_action_quiet( ( buff != NULL ) && ( length <= TX_MAX_DATA_LENGTH - 5 ), exit, err = kParamErr);
  pBatteryData->pTxData = buff;
  if( *pBatteryData->pTxData != 0x7E )
  {
    *pBatteryData->pTxData = 0x7E;//'~'
  }
  while( check_num -- )
  {
    check_sum += *++pBatteryData->pTxData;
  }
  check_sum = ~check_sum + 1;
  WriteBig16( &check_sum_big, check_sum );
  
  convert_str = DataToHexString( (uint8_t *)&check_sum_big, 2 );
  memcpy( ++pBatteryData->pTxData, convert_str, 4 );
  free(convert_str);
  
  pBatteryData->pTxData += 4;
  *pBatteryData->pTxData = 0x0D;//'\r'
  
  err = MicoUartSend( BATT_UART, (uint8_t *)pBatteryData->tx_data_buff, pBatteryData->pTxData - pBatteryData->tx_data_buff + 1 );
  memset( (uint8_t *)pBatteryData->tx_data_buff, 0x00, TX_MAX_DATA_LENGTH );
  
exit:
  return err;
}

static void irq_clear_err(UART_HandleTypeDef *huart)
{
  uint32_t tmp_flag = 0, tmp_it_source = 0;

  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
  /* UART parity error interrupt occurred ------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  { 
    __HAL_UART_CLEAR_PEFLAG(huart);
    
    huart->ErrorCode |= HAL_UART_ERROR_PE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
  /* UART frame error interrupt occurred -------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  { 
    __HAL_UART_CLEAR_FEFLAG(huart);
    
    huart->ErrorCode |= HAL_UART_ERROR_FE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
  /* UART noise error interrupt occurred -------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  { 
    __HAL_UART_CLEAR_NEFLAG(huart);
    
    huart->ErrorCode |= HAL_UART_ERROR_NE;
  }
  
  tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
  /* UART Over-Run interrupt occurred ----------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  { 
    __HAL_UART_CLEAR_OREFLAG(huart);
    
    huart->ErrorCode |= HAL_UART_ERROR_ORE;
  }
}
void battery_data_recieved( void )
{
  irq_clear_err( platform_uart_drivers[BATT_UART].uart_handle );
  
  if( pBatteryData->isDetectedEOI )
  {
    (uint8_t)(platform_uart_drivers[BATT_UART].uart_handle->Instance->DR & (uint8_t)0x00FF);
    return;
  }
  
  *pBatteryData->pRxData++ = (uint8_t)(platform_uart_drivers[BATT_UART].uart_handle->Instance->DR & (uint8_t)0x00FF);

  if( !pBatteryData->isDetectedSOI )
  {
    if( *(pBatteryData->pRxData - 1) == 0x7E )
    {
      pBatteryData->isDetectedSOI = 1;
    }
    else
    {
      pBatteryData->pRxData--;
    }
  }
  if( pBatteryData->isDetectedSOI && !pBatteryData->isDetectedEOI )
  {
    if( *(pBatteryData->pRxData - 1) == 0x0D )
    {
      pBatteryData->isDetectedEOI = 1;
    }
  }
  
  if( pBatteryData->pRxData > pBatteryData->rx_data_buff + RX_MAX_DATA_LENGTH - 1 )
  {
    pBatteryData->isDetectedSOI = 0;
    //pBatteryData->pRxData = (uint8_t *)pBatteryData->rx_data_buff;
  }
}

static uint8_t information[100];

OSStatus isDataCheckRight( uint8_t const* in_str, int32_t str_len, uint8_t* const out_hex)
{
  uint16_t compute_check = 0;
  uint16_t convert_check = 0;
  battery_protocol_t *read_data = (battery_protocol_t *)out_hex;
  OSStatus err = kNoErr;
  char *p_str = (char *)in_str + 1;
  uint8_t* out_one_hex = out_hex + 1;
  uint16_t lenid;
  char *info_end;
  
  require_action_quiet( in_str, exit, err = kParamErr);
  
  *information = *in_str;
  while( str_len >= 0 )
  {
    string_to_unsigned(p_str, 2, (uint32_t *)out_one_hex, 1);
    p_str += 2;
    out_one_hex ++;
    str_len -= 2;
  }
  
  lenid = getLengthFromLengthData( read_data->length_h << 8 | read_data->length_l );
  
  p_str = (char *)in_str + 1;
  
  compute_check = 0;
  info_end = (char *)(in_str + 13 + lenid);
  while( p_str < info_end )
  {
    compute_check += *p_str++;
  }
  compute_check = ~(uint16_t)(compute_check) + 1;
  string_to_unsigned(p_str, 4, (uint32_t *)&convert_check, 1);
  
  if( compute_check != convert_check )
  {
    err = kGeneralErr;
    battery_log("check data not pass");
  }
  
exit:
  return err;
}

battery_pack_t battery_pack;

void  battery_parse_rx_buffer( const uint8_t * const hex_rx_buf )
{
  uint8_t battery_m;
  uint8_t temperture_n;
  battery_protocol_t *read_data = (battery_protocol_t *)hex_rx_buf;
  uint8_t* ptr_rx = (uint8_t *)hex_rx_buf + 7;

  if( read_data->cid2 != 0x00 )
  {
    return;
  }
  if( *ptr_rx++ != 0x11 )//Data Flag
  {
    //return;
  }
  if( *ptr_rx++ != 0x01 )//address
  { 
    //return;
  }
  battery_m = *ptr_rx;//battery M
  ptr_rx += 2*battery_m;
  temperture_n = *++ptr_rx;//temperture N
  ptr_rx += 2*temperture_n;
  ptr_rx += 1;
  battery_pack.pack_current = ReadBig16( ptr_rx );
  ptr_rx += 2;
  battery_pack.pack_voltage = ReadBig16( ptr_rx );
  ptr_rx += 2;
  battery_pack.pack_current_soc = ReadBig16( ptr_rx );
  ptr_rx += 2;
  *ptr_rx ++;//user define num
  battery_pack.pack_totoal_soc = ReadBig16( ptr_rx );
  ptr_rx += 2;
  battery_pack.pack_recharge_cycle = ReadBig16( ptr_rx );
  
  if(battery_pack.pack_totoal_soc > 0)
  {
      uint16_t percentage = 0;
      percentage= battery_pack.pack_current_soc * 1000 / battery_pack.pack_totoal_soc;
      percentage += 5;  //percentage: rounding
      battery_pack.percentage = percentage/10;
      if(battery_pack.percentage > 100)
      {
          battery_pack.percentage = 100;
      }
  }
}
void PrintBatInfo(void)
{
    battery_log("battery current is %d",battery_pack.pack_current);
    battery_log("battery voltage is %d",battery_pack.pack_voltage);
    battery_log("battery recharge cycle is %d",battery_pack.pack_recharge_cycle);
    battery_log("battery total soc is %d",battery_pack.pack_totoal_soc);
    battery_log("battery current soc is %d",battery_pack.pack_current_soc);  
    battery_log("battery percent is %d",battery_pack.pack_current_soc * 100 / battery_pack.pack_totoal_soc);           
}

#define BAT_SOI     0x7E
#define BAT_EOI     0x0D
#define BATTERY_COM_ERR_DEBOUNCE_CNT    3
#define BATTERY_READ_PERIOD             (3000/SYSTICK_PERIOD)
#define BATTERY_FIRST_READ_PERIOD       (2800/SYSTICK_PERIOD)
static uint32_t battery_period_start_time;
static uint8_t battery_com_err_cnt = 0;
extern void UploadBatInfo(void);
void battery_period( void )
{
    uint8_t recvDataLength;
    //uint32_t rcv_len = RcvDmaDataLengthEx( BATT_UART , RX_MAX_DATA_LENGTH);

    if( pBatteryData->isDetectedEOI )
    {
        pBatteryData->isDetectedEOI = 0;
        recvDataLength = pBatteryData->pRxData - pBatteryData->rx_data_buff;
        pBatteryData->pRxData = (uint8_t *)pBatteryData->rx_data_buff;
        pBatteryData->isDetectedSOI = 0;
        //MicoUartSend( STDIO_UART, (uint8_t *)pBatteryData->rx_data_buff, recvDataLength );
        //battery_log("reveiced right data:%s\r\n", pBatteryData->rx_data_buff);
        if( !isDataCheckRight( pBatteryData->rx_data_buff, recvDataLength, information ) )
        {
            //battery_log("check data pass");
            if( pBatteryData->cmd_type == 0x42 )
            {
                battery_parse_rx_buffer(information);
                battery_com_err_cnt = 0;
                battery_pack.com_status = true;
            }
            pBatteryData->cmd_type = 0x00;
            StartDmaReciveEx( BATT_UART, (uint8_t *)rx_data , RX_MAX_DATA_LENGTH);
        }
    }

    if( os_get_time() + BATTERY_FIRST_READ_PERIOD - battery_period_start_time >= BATTERY_READ_PERIOD  )
    {   
        static uint8_t err_cnt = 0;
        battery_period_start_time = os_get_time() + BATTERY_FIRST_READ_PERIOD;  
        battery_com_err_cnt++;
        
        battery_read_infomation(); 
        //battery_read_warm_info();
        //battery_get_system_param();
        //MicoUartSend(BATT_UART, test_buf, sizeof(test_buf) );
        
        if(battery_com_err_cnt >= BATTERY_COM_ERR_DEBOUNCE_CNT)
        {
            battery_log(" CAN NOT GET BATTERY INFO ! ! !\r\n");
            if(err_cnt++ >= 3)
            {
                battery_pack.com_status = false;
                err_cnt = 0;
                UploadBatInfo();
            }
            
        }
        else
        {
            //PrintBatInfo();
            err_cnt = 0;
        }
        
    }
}

