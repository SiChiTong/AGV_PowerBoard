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

#define CAN_PROTOCOL_TASK_STK_SIZE  1024
extern OS_STK can_protocol_task_stk[CAN_PROTOCOL_TASK_STK_SIZE];
void can_protocol_task(void *pdata);


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
#define CAN_SOURCE_ID_READ_VERSION  0x01
#define CAN_READ_DATA               0x80
#define CAN_SOURCE_ID_GET_SLIDER_VALUE      0x81


#define CAN_ONE_FRAME_DATA_LENTH    7
#define CAN_SEG_NUM_MAX             64
#define CAN_LONG_FRAME_LENTH_MAX    (CAN_ONE_FRAME_DATA_LENTH*CAN_SEG_NUM_MAX)
typedef struct
{
    uint32_t can_id;
    uint32_t start_time; 
    uint16_t used_len;
    uint8_t rcv_buf[CAN_LONG_FRAME_LENTH_MAX];   
}CAN_RCV_BUFFER_T;

typedef uint8_t (*GetOneFreeBufFn)(void);
typedef uint8_t (*GetTheBufByIdFn)(uint32_t);
typedef void (*FreeBufFn)(uint8_t);

#define CAN_LONG_BUF_NUM    2   // value can not be 0 ! !
typedef struct
{
    CAN_RCV_BUFFER_T can_rcv_buf[CAN_LONG_BUF_NUM];
    GetOneFreeBufFn GetOneFreeBuf; 
    GetTheBufByIdFn GetTheBufById;
    FreeBufFn FreeBuf;
}CAN_LONG_BUF_T;

void RxMsgHandle(uint32_t ID,uint8_t* pdata);

void CM_CAN_Init(void);
void RxMsgHandle(uint32_t ID,uint8_t* pdata);

void can_protocol_period( void );

void CanLongBufInit(void);


#endif
