/*
 *  Author: Adam Huang
 *  Date:2017/01/23
 */

#ifndef __CAN_PROTOOCOL_H
#define __CAN_PROTOOCOL_H

#include "mico.h"

#define CAN_USED    CAN1


#define CAN_NOAH_PB_ID   0x52


#define SOURCE_ID_PREPARE_UPDATE        0x10
#define SOURCE_ID_TRANSMIT_UPDATE       0x11
#define SOURCE_ID_CHECK_TRANSMIT        0x12


//////  function id define  //////
#define CAN_FUN_ID_RESET        0x06
#define CAN_FUN_ID_WRITE        0x01
#define CAN_FUN_ID_READ         0x02
#define CAN_FUN_ID_TRIGGER      0x03


//////  source id define  //////
#define CAN_SOURCE_ID_READ_VERSION      0x01

#define CAN_SOURCE_ID_SET_MODULE_STATE              0x81
#define CAN_SOURCE_ID_GET_MODULE_STATE              0x82
#define CAN_SOURCE_ID_GET_SYS_STATE                 0x83
#define CAN_SOURCE_ID_GET_ERR_STATE                 0x84
#define CAN_SOURCE_ID_GET_BAT_STATE                 0x85
#define CAN_SOURCE_ID_GET_ADC_DATA                  0x86
#define CAN_SOURCE_ID_SET_IR_LED_LIGHTNESS          0x87
#define CAN_SOURCE_ID_GET_IR_LED_LIGHTNESS          0x88
#define CAN_SOURCE_ID_SET_LED_EFFECT                0x89
#define CAN_SOURCE_ID_POWER_OFF_SIGNAL              0x8a
#define CAN_SOURCE_ID_REMOTE_POWRER_CTRL            0x8b
#define CAN_SOURCE_ID_GET_SERIALS_LEDS_VERSION      0x8c

#define CAN_FIFO_SIZE                   250



typedef struct
{
    uint8_t fun_id;
    uint16_t len;
    uint8_t *pdata;
}can_tx_data_t;


typedef union
{
    struct
    {
        uint32_t source_id  : 8;
        uint32_t func_id   : 4;
        uint32_t ack       : 1;
        uint32_t dest_mac_id : 8;
        uint32_t src_mac_id  : 8;
        uint32_t res       : 3;
    }canx_id_t;
    uint32_t  canx_id;
}can_id_union;

typedef union
{
    struct
    {
        uint8_t seg_num  : 6;
        uint8_t seg_polo : 2;
        uint8_t data[7];
    }can_data_t;
    uint8_t can_data[8];
}can_data_union;

#define CAN_ONE_FRAME_DATA_LENTH    7
#define CAN_SEG_NUM_MAX             64
#define CAN_LONG_FRAME_LENTH_MAX    (CAN_ONE_FRAME_DATA_LENTH*CAN_SEG_NUM_MAX)

typedef struct
{
    uint32_t can_id;
    uint32_t start_time;
    uint16_t used_len;
    uint8_t rcv_buf[CAN_LONG_FRAME_LENTH_MAX];
}can_rcv_buf_t;

typedef uint8_t (*get_one_free_buf_fn)(void);
typedef uint8_t (*get_the_buf_by_id_fn)(uint32_t);
typedef void (*free_buf_fn)(uint8_t);

#define CAN_LONG_BUF_NUM    2

typedef struct
{
    can_rcv_buf_t can_rcv_buf[CAN_LONG_BUF_NUM];
    get_one_free_buf_fn get_one_free_buf;
    get_the_buf_by_id_fn get_the_buf_by_id;
    free_buf_fn free_buf;
}can_long_buf_t;


void upload_adc_data(void);
void can_protocol_period( void );
void can_long_buf_init(void);

#endif
