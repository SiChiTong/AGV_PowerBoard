/**
******************************************************************************
* @file    can.c
* @author  Kaka.Xie
* @brief   This file provides CAN driver.
******************************************************************************
*/

#ifndef __CAN_PROTOCOL_H
#define __CAN_PROTOCOL_H
#include "stm32f10x.h"
#include "ucos_ii.h"
#include "can_fifo.h"
#define CAN_PROTOCOL_TASK_STK_SIZE  1024
#define CAN_SEND_TASK_STK_SIZE      1024
extern OS_STK can_protocol_task_stk[CAN_PROTOCOL_TASK_STK_SIZE];
extern OS_STK can_send_task_stk[CAN_SEND_TASK_STK_SIZE];
void can_protocol_task(void *pdata);
void can_send_task(void *pdata);



#define HW_VERSION                      "M30_MAINBOARD_V0.2"
#define SW_VERSION                      "M30C001M08B101"
#define PROTOCOL_VERSION                "20170619P0001"

#define CMD_NOT_FOUND   0



#define CAN_USED    CAN1
#define CAN_ID          (0x434D0000)      //CM
#define CAN_LOCAL_ID    (0x06)            //local ID


#define SOURCE_ID_PREPARE_UPDATE        0x10
#define SOURCE_ID_TRANSMIT_UPDATE       0x11
#define SOURCE_ID_CHECK_TRANSMIT        0x12

//////  function id define  //////
#define CAN_FUN_ID_RESERVE_0    0x00

#define CAN_FUN_ID_WRITE        0x01
#define CAN_FUN_ID_READ         0x02
#define CAN_FUN_ID_TRIGGER      0x03

#define CAN_FUN_ID_RESERVE_4    0x04
#define CAN_FUN_ID_RESERVE_5    0x05

#define CAN_FUN_ID_RESET        0x06

//////  source id define  //////
#define CAN_SOURCE_ID_READ_VERSION                  0x01
#define CAN_SOURCE_ID_SET_MODULE_STATE              0x81
#define CAN_SOURCE_ID_GET_MODULE_STATE              0x82
#define CAN_SOURCE_ID_GET_SYS_STATE                 0x83
#define CAN_SOURCE_ID_GET_ERR_STATE                 0x84
#define CAN_SOURCE_ID_GET_BAT_STATE                 0x85

#define CAN_SOURCE_ID_SET_IR_LED_LIGHTNESS          0x87
#define CAN_SOURCE_ID_GET_IR_LED_LIGHTNESS          0x88
#define CAN_SOURCE_ID_SET_LED_EFFECT                0x89
#define CAN_SOURCE_ID_POWER_OFF_SIGNAL              0x8a
#define CAN_SOURCE_ID_REMOTE_POWRER_CTRL            0x8b
#define CAN_SOURCE_ID_GET_SERIALS_LEDS_VERSION      0x8c
#define CAN_SOURCE_ID_EVENT_BUTTON                  0x8d
#define CAN_SOURCE_ID_SET_LED_STATUS                0x8e
#define CAN_SOURCE_ID_GET_DEV_ID                    0x8f




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

#define CAN_LONG_BUF_NUM    2   // value can not be 0 ! !
typedef struct
{
    can_rcv_buf_t can_rcv_buf[CAN_LONG_BUF_NUM];
    get_one_free_buf_fn get_one_free_buf;
    get_the_buf_by_id_fn get_the_buf_by_id;
    free_buf_fn free_buf;
}can_long_buf_t;


#define CAN_SEND_BUF_SIZE           10
#define CAN_SEND_BUF_QUEUE_NUM      CAN_SEND_BUF_SIZE

#define CAN_RCV_BUF_SIZE           10
#define CAN_RCV_BUF_QUEUE_NUM      CAN_RCV_BUF_SIZE

#define CAN_UPLOAD_ACK_SIZE         10
#define CAN_UPLOAD_ACK_QUEUE_NUM    CAN_UPLOAD_ACK_SIZE

#define CAN_BUF_INSIDE_DATA_SIZE    64
typedef struct
{
    uint32_t id;
    uint8_t data_len;
    uint8_t data[CAN_BUF_INSIDE_DATA_SIZE];
}__attribute__ ((__packed__)) can_buf_t;



typedef struct
{
    uint8_t serial_num;
    can_id_union id;
}can_upload_ack_t;

extern can_buf_t can_send_buf_mem[10][1];
extern OS_MEM *can_send_buf_mem_handle;

extern OS_EVENT *can_send_buf_queue_handle;
extern void* can_send_buf_queue_p[CAN_SEND_BUF_QUEUE_NUM];



extern can_pkg_t can_rcv_buf_mem[CAN_RCV_BUF_SIZE][1];
extern OS_MEM *can_rcv_buf_mem_handle;

extern OS_EVENT *can_rcv_buf_queue_handle;
extern void* can_rcv_buf_queue_p[CAN_RCV_BUF_QUEUE_NUM];


extern can_upload_ack_t can_upload_ack_mem[CAN_UPLOAD_ACK_SIZE][1];
extern OS_MEM *can_upload_ack_mem_handle;

extern OS_EVENT *can_upload_ack_queue_handle;
extern void* can_upload_ack_queue_p[CAN_UPLOAD_ACK_QUEUE_NUM];

void can_protocol_period( void );

void can_long_buf_init(void);
int send_can_msg(can_buf_t *can_msg);

void upload_sys_state(void);
void upload_conveyor_belt_status(uint8_t status);
void upload_event_button_state(void);

#endif
