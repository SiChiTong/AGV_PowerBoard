/*
 *  Author: Kaka Xie
 *  brief:  battery power processing
 */
#include "battery.h"
#include "delay.h"
#include "platform.h"
#include <stdio.h>
#include "usart.h"
#include <string.h>


uint8_t string_to_unsigned(const char* string, uint8_t str_length, uint32_t* value_out, uint8_t is_hex);

void bat_uart_init(void)
{
    uart_4_init(9600);
//    battery_protocol_init();
}


#if 1

#define RX_MAX_DATA_LENGTH      512
#define TX_MAX_DATA_LENGTH      100
volatile uint8_t rx_data[RX_MAX_DATA_LENGTH];
volatile uint8_t tx_data[TX_MAX_DATA_LENGTH];

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


int check_and_send(uint8_t *buff, uint32_t length);
void battery_activate(void);

void battery_protocol_init(void)
{
    if(!pBatteryData)
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

uint16_t getLengthDataFromLength(uint16_t data_length)
{
    length_data_union length_data_u;
    uint8_t length_check = 0;
    uint16_t length = data_length;

    if(length > 0xFFF)
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

uint16_t getLengthFromLengthData(uint16_t length_data)
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

    if(length_check != length_data_u.length_struct.lchecksum)
    {
        printf("length check isn't right");
    }

    return length;
}

char* DataToHexString( const uint8_t *inBuf, size_t inBufLen, char *out_buf )
{

    require_quiet(inBuf, error);
    require_quiet(inBufLen > 0, error);
#if 0
    char* buf_str = NULL;
    char* buf_ptr;
    buf_str = (char*) malloc (2*inBufLen + 1);
    require(buf_str, error);
    buf_ptr = buf_str;
    uint32_t i;
    for (i = 0; i < inBufLen; i++) buf_ptr += sprintf(buf_ptr, "%02X", inBuf[i]);
    *buf_ptr = '\0';
    return buf_str;
#endif
    uint32_t i;
    for (i = 0; i < inBufLen; i++)
    {
        sprintf(&out_buf[2 * i], "%02X", inBuf[i]);
    }
    out_buf[2*inBufLen] = '\0';

error:
//    if ( buf_str ) free( buf_str );
    return NULL;
}

uint8_t string_to_unsigned(const char* string, uint8_t str_length, uint32_t* value_out, uint8_t is_hex)
{
    uint8_t i = 0;
    uint32_t value = 0;
    uint32_t product = 1;
    uint8_t tmp = 0;
    if(str_length > 8)
    {
        return 0;
    }
//    for(i = 0; i < str_length - 1; i++)
//    {
//        product *= 10;
//    }
    for(i = str_length; i > 0; i--)
    {
        tmp = string[i - 1];
        if(is_hex)
        {
            product *= 0x10;
            if((tmp >= '0') && (tmp <= '9'))
            {
                value += product * (tmp - '0');
            }
            else if((tmp >= 'a') && (tmp <= 'f'))
            {
                value += product * ((tmp - 'a') + 0x0a);
            }
            else if((tmp >= 'A') && (tmp <= 'F'))
            {
                value += product * ((tmp - 'A') + 0x0a);
            }
            else
            {
                return 0;
            }
        }
        else
        {
            /*
            TODO:
            */
            return 0;
        }
    }
    *value_out = (value / 0x10)/* & 0xff*/;
    return 1;
}

int battery_common_read(uint8_t cid2, uint8_t* info, uint16_t info_len)
{
    int err = kNoErr;
    battery_protocol_t read_data;
    char convert_str[6*2 + 1 + 2];
    //static uint8_t test = 0x10;
    require_action_quiet((info != NULL) && (info_len <= READ_INFO_LENGTH), exit, err = kParamErr);

    read_data.soi = 0x7E;//'~'
    read_data.ver = 0x20;//test++;//
    read_data.adr = 0x01;
    read_data.cid1 = 0x46;
    read_data.cid2 = cid2;
    WriteBig16(&read_data.length_h, getLengthDataFromLength(info_len));

//    convert_str = DataToHexString((uint8_t *)&read_data.ver, 6, convert_str);
    DataToHexString((uint8_t *)&read_data.ver, 6, convert_str);
    memcpy((uint8_t *)pBatteryData->tx_data_buff + 1, convert_str, 2*6);
//    free(convert_str);

    read_data.info = info;
    memcpy((uint8_t *)pBatteryData->tx_data_buff + 13, read_data.info, info_len);
    err = check_and_send((uint8_t *)pBatteryData->tx_data_buff, 13 + info_len);

exit:
    return err;
}

void battery_activate(void)
{
    *read_info_buff = '8';
    *(read_info_buff + 1) = '5';
    *(read_info_buff + 2) = '2';
    *(read_info_buff + 3) = '0';
    pBatteryData->cmd_type = 0x82;
    battery_common_read(0x82, read_info_buff, 4);
}

void battery_read_infomation(void)
{
    *read_info_buff = '0';
    *(read_info_buff + 1) = '1';
    pBatteryData->cmd_type = 0x42;
    battery_common_read(0x42, read_info_buff, 2);
}

void battery_read_warm_info(void)
{
    *read_info_buff = '0';
    *(read_info_buff + 1) = '1';
    pBatteryData->cmd_type = 0x44;
    battery_common_read(0x44, read_info_buff, 2);
}

void battery_get_system_param(void)
{
    pBatteryData->cmd_type = 0x47;
    battery_common_read(0x47, read_info_buff, 0);
}

int check_and_send(uint8_t *buff, uint32_t length)
{
    int err = kNoErr;
    uint32_t check_num = length - 1;
    uint16_t check_sum = 0;
    uint16_t check_sum_big = 0;
    char convert_str[2 * 2 + 1 + 2];

    require_action_quiet((buff != NULL) && (length <= TX_MAX_DATA_LENGTH - 5), exit, err = kParamErr);
    pBatteryData->pTxData = buff;
    if(*pBatteryData->pTxData != 0x7E)
    {
        *pBatteryData->pTxData = 0x7E;//'~'
    }
    while(check_num --)
    {
        check_sum += *++pBatteryData->pTxData;
    }
    check_sum = ~check_sum + 1;
    WriteBig16(&check_sum_big, check_sum);

//    convert_str = DataToHexString((uint8_t *)&check_sum_big, 2);
    DataToHexString((uint8_t *)&check_sum_big, 2, convert_str);
    memcpy(++pBatteryData->pTxData, convert_str, 4);
//    free(convert_str);

    pBatteryData->pTxData += 4;
    *pBatteryData->pTxData = 0x0D;//'\r'

    err = uart_4_send((uint8_t *)pBatteryData->tx_data_buff, pBatteryData->pTxData - pBatteryData->tx_data_buff + 1);
    memset((uint8_t *)pBatteryData->tx_data_buff, 0x00, TX_MAX_DATA_LENGTH);

exit:
    return err;
}




void battery_data_recieved(uint8_t data)
{
//    irq_clear_err(platform_uart_drivers[BATT_UART].uart_handle);

    if(pBatteryData->isDetectedEOI)
    {
//        (uint8_t)(platform_uart_drivers[BATT_UART].uart_handle->Instance->DR & (uint8_t)0x00FF);
        return;
    }

    *pBatteryData->pRxData++ = data;

    if(!pBatteryData->isDetectedSOI)
    {
        if(*(pBatteryData->pRxData - 1) == 0x7E)
        {
            pBatteryData->isDetectedSOI = 1;
        }
        else
        {
            pBatteryData->pRxData--;
        }
    }
    if(pBatteryData->isDetectedSOI && !pBatteryData->isDetectedEOI)
    {
        if(*(pBatteryData->pRxData - 1) == 0x0D)
        {
            pBatteryData->isDetectedEOI = 1;
        }
    }

    if(pBatteryData->pRxData > pBatteryData->rx_data_buff + RX_MAX_DATA_LENGTH - 1)
    {
        pBatteryData->isDetectedSOI = 0;
        //pBatteryData->pRxData = (uint8_t *)pBatteryData->rx_data_buff;
    }
}

static uint8_t information[100];

int isDataCheckRight(uint8_t const* in_str, int32_t str_len, uint8_t* const out_hex)
{
    uint16_t compute_check = 0;
    uint16_t convert_check = 0;
    battery_protocol_t *read_data = (battery_protocol_t *)out_hex;
    int err = kNoErr;
    char *p_str = (char *)in_str + 1;
    uint8_t* out_one_hex = out_hex + 1;
    uint16_t lenid;
    char *info_end;

    require_action_quiet(in_str, exit, err = kParamErr);

    *information = *in_str;
    while(str_len >= 0)
    {
        string_to_unsigned(p_str, 2, (uint32_t *)out_one_hex, 1);
        p_str += 2;
        out_one_hex ++;
        str_len -= 2;
    }

    lenid = getLengthFromLengthData(read_data->length_h << 8 | read_data->length_l);

    p_str = (char *)in_str + 1;

    compute_check = 0;
    info_end = (char *)(in_str + 13 + lenid);
    while(p_str < info_end)
    {
        compute_check += *p_str++;
    }
    compute_check = ~(uint16_t)(compute_check) + 1;
    string_to_unsigned(p_str, 4, (uint32_t *)&convert_check, 1);

    if(compute_check != convert_check)
    {
        err = kGeneralErr;
        printf("check data not pass");
    }

exit:
    return err;
}

battery_pack_t battery_pack;

void  battery_parse_rx_buffer(const uint8_t * const hex_rx_buf)
{
    uint8_t battery_m;
    uint8_t temperture_n;
    battery_protocol_t *read_data = (battery_protocol_t *)hex_rx_buf;
    uint8_t* ptr_rx = (uint8_t *)hex_rx_buf + 7;

    if(read_data->cid2 != 0x00)
    {
        return;
    }
    if(*ptr_rx++ != 0x11)//Data Flag
    {
        //return;
    }
    if(*ptr_rx++ != 0x01)//address
    {
        //return;
    }
    battery_m = *ptr_rx;//battery M
    ptr_rx += 2*battery_m;
    temperture_n = *++ptr_rx;//temperture N
    ptr_rx += 2*temperture_n;
    ptr_rx += 1;
    battery_pack.pack_current = ReadBig16(ptr_rx);
    ptr_rx += 2;
    battery_pack.pack_voltage = ReadBig16(ptr_rx);
    ptr_rx += 2;
    battery_pack.pack_current_soc = ReadBig16(ptr_rx);
    ptr_rx += 2;
    *ptr_rx ++;//user define num
    battery_pack.pack_totoal_soc = ReadBig16(ptr_rx);
    ptr_rx += 2;
    battery_pack.pack_recharge_cycle = ReadBig16(ptr_rx);

    if(battery_pack.pack_totoal_soc > 0)
    {
        uint16_t percentage = 0;
        percentage = battery_pack.pack_current_soc * 1000 / battery_pack.pack_totoal_soc;
        percentage += 5;  //percentage: rounding
        battery_pack.percentage = percentage / 10;

        //if(battery_pack.percentage > 100)
        if(battery_pack.pack_current_soc >= battery_pack.pack_totoal_soc)
        {
            battery_pack.percentage = 100;
        }
    }
}

void print_bat_info(void)
{
    printf("battery current is %d",battery_pack.pack_current);
    printf("battery voltage is %d",battery_pack.pack_voltage);
    printf("battery recharge cycle is %d",battery_pack.pack_recharge_cycle);
    printf("battery total soc is %d",battery_pack.pack_totoal_soc);
    printf("battery current soc is %d",battery_pack.pack_current_soc);
    printf("battery percent is %d",battery_pack.pack_current_soc * 100 / battery_pack.pack_totoal_soc);
}

#define BAT_SOI     0x7E
#define BAT_EOI     0x0D
#define BATTERY_COM_ERR_DEBOUNCE_CNT    3
static uint8_t battery_com_err_cnt = 0;
extern void upload_bat_info(void);
void battery_period(void)
{
    uint8_t recvDataLength;

    if(pBatteryData->isDetectedEOI)
    {
        pBatteryData->isDetectedEOI = 0;
        recvDataLength = pBatteryData->pRxData - pBatteryData->rx_data_buff;
        pBatteryData->pRxData = (uint8_t *)pBatteryData->rx_data_buff;
        pBatteryData->isDetectedSOI = 0;

        if(!isDataCheckRight(pBatteryData->rx_data_buff, recvDataLength, information))
        {
            if(pBatteryData->cmd_type == 0x42)
            {
                battery_parse_rx_buffer(information);
                battery_com_err_cnt = 0;
                battery_pack.com_status = TRUE;
            }
            pBatteryData->cmd_type = 0x00;
        }
    }

    battery_read_infomation();

    if(battery_com_err_cnt++ >= BATTERY_COM_ERR_DEBOUNCE_CNT)
    {
        printf(" CAN NOT GET BATTERY INFO ! ! !\r\n");
        {
            battery_pack.com_status = FALSE;
//                upload_bat_info();
        }
        battery_com_err_cnt = 0;
    }
}
#endif

