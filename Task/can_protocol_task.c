/**
******************************************************************************
* @file    can.c 
* @author  Kaka.Xie
* @brief   This file provides CAN protocol functions.
******************************************************************************

******************************************************************************
*/ 
#include "can_protocol_task.h"
#include "delay.h"
#include "platform.h"
#include <string.h>
#include <stdio.h>
#include "can.h"
#include "can_fifo.h"


//#define CanProtocolLog(format, ...)  custom_log("can protocol", format, ##__VA_ARGS__)

CanRxMsg RxMessage;
CanTxMsg TxMessage;
uint8_t CanTxdataBuff[CAN_LONG_FRAME_LENTH_MAX] = {0};


uint16_t CmdProcessing(CAN_ID_UNION *id, uint8_t *data_in, uint16_t data_len, uint8_t *data_out)
{
    id->CanID_Struct.ACK = 1;
    switch(id->CanID_Struct.FUNC_ID)
    {
        case CAN_FUN_ID_RESET:
//          platform_mcu_reset();
          break;
        case CAN_FUN_ID_WRITE:
        case CAN_FUN_ID_READ:
          switch(id->CanID_Struct.SourceID)
          {
            case CAN_SOURCE_ID_READ_VERSION:
              //SW_VERSION
              break;
            case CAN_READ_DATA:
            
                return 1;
            default :
              break;
          }       
        default: 
          break;
    }  
    return 0;
}


#define ONLYONCE       0x00
#define BEGIN         0x01
#define TRANSING       0x02
#define END            0x03


void Can1_TX(uint32_t CANx_ID,uint8_t* pdata,uint16_t len)
{
  //return ;
	uint16_t t_len;
	uint16_t roundCount;
	uint8_t modCount;
	CAN_DATA_UNION TxMsg = {0};
	//CanTxMsgTypeDef *TxMessage = platform_can_drivers[can_type].handle->pTxMsg;
    
	t_len = len;
	roundCount = t_len/7;
	modCount = t_len%7;
	
	TxMessage.ExtId = CANx_ID;
	TxMessage.IDE   = CAN_ID_EXT;
	TxMessage.RTR   = CAN_RTR_DATA;
	//if(roundCount <= 1)
    if(t_len <= 7)
    {
        TxMsg.CanData_Struct.SegPolo = ONLYONCE;
        TxMessage.DLC = t_len+1;		
        
        
        memcpy(&TxMessage.Data[1],pdata,t_len);
        TxMessage.Data[0] = TxMsg.CanData[0];
        
        if((CAN_USED->TSR&0x1C000000))
        {
            CAN_Transmit(CAN1, &TxMessage);//
        }
        else
        {
            printf("TX busy ! \r\n");
        }
        return ;
    }
    
	{
		int num;
        {
            for(num = 0; num < roundCount; num++)
            {		
        //SET SEGPOLO				
                if( num == 0)
                {
                    TxMsg.CanData_Struct.SegPolo = BEGIN;
                }
                else
                {
                    TxMsg.CanData_Struct.SegPolo = TRANSING;
                }
                
                if( modCount == 0 && num == roundCount-1)
                {
                    TxMsg.CanData_Struct.SegPolo = END;
                }
                            
                TxMsg.CanData_Struct.SegNum = num;
                memcpy(TxMsg.CanData_Struct.Data, &pdata[num*7], 7);
                memcpy(TxMessage.Data, TxMsg.CanData, 8);
                TxMessage.DLC = 8;
                if((CAN_USED->TSR&0x1C000000))
                {
                    CAN_Transmit(CAN1, &TxMessage);//
                }
                else
                {
                    printf("TX busy ! \r\n");
                }
                
                //TRANSMIT LAST MSG
                if( modCount !=0 && num == roundCount-1 )
                {
                    num++;
                    TxMsg.CanData_Struct.SegPolo = END;
                    TxMsg.CanData_Struct.SegNum = num;
                    memcpy(TxMsg.CanData_Struct.Data,&pdata[num*7],modCount);
                    memcpy(TxMessage.Data,TxMsg.CanData,modCount+1);
                    TxMessage.DLC = modCount+1;
                    if((CAN_USED->TSR&0x1C000000))
                    {
                        CAN_Transmit(CAN1, &TxMessage);//
                    }
                    else
                    {
                        printf("TX busy ! \r\n");
                    }
                }
            }
            
        }
        
	}
}



CAN_LONG_BUF_T can_long_frame_buf_ram;
CAN_LONG_BUF_T *can_long_frame_buf = &can_long_frame_buf_ram;

#define CAN_LONG_BUF_FULL   0xff
static uint8_t GetOneFreeBuf(void)
{
    for(uint8_t i = 0; i < CAN_LONG_BUF_NUM; i++)
    {
        if(can_long_frame_buf->can_rcv_buf[i].used_len == 0)
        {
            return i;
        }
    }
    return CAN_LONG_BUF_FULL;
}
static void FreeBuf(uint8_t index)
{
    can_long_frame_buf->can_rcv_buf[index].can_id = 0;
    can_long_frame_buf->can_rcv_buf[index].used_len = 0;
}
#define CAN_BUF_NO_THIS_ID      0xfe
static uint8_t GetTheBufById(uint32_t id)
{
    for(uint8_t i = 0; i < CAN_LONG_BUF_NUM; i++)
    {
        if(id == can_long_frame_buf->can_rcv_buf[i].can_id)
        {
            return i;
        }
    }
    return CAN_BUF_NO_THIS_ID;
}
void CanLongBufInit(void)
{ 
    can_long_frame_buf->GetOneFreeBuf = GetOneFreeBuf;
    can_long_frame_buf->GetTheBufById = GetTheBufById;
    can_long_frame_buf->FreeBuf = FreeBuf;
    can_fifo_init(can_fifo, can_pkg, CAN_FIFO_SIZE);
}

#define CAN_LONG_FRAME_TIME_OUT     5 * OS_TICKS_PER_SEC
#define CAN_COMM_TIME_OUT           5 * OS_TICKS_PER_SEC
uint32_t can_comm_start_time;

#define CAN_COM_TIME_OUT    5 * OS_TICKS_PER_SEC
static uint32_t can_com_start_time = 0;




OS_STK can_protocol_task_stk[CAN_PROTOCOL_TASK_STK_SIZE];

void can_protocol_task(void *pdata)
{
    static CAN_ID_UNION id = {0};
    static CAN_DATA_UNION rx_buf = {0};

    while(1)
    {
        can_pkg_t can_pkg_tmp = {0};
        uint8_t buf_index = 0;
        uint8_t seg_polo = 0;
        uint8_t seg_num = 0;
        uint16_t tx_len = 0;
        uint8_t rx_len = 0;

        if(is_can_fifo_empty(can_fifo) == FALSE)
        {
            get_can_pkg_from_fifo(can_fifo, &can_pkg_tmp);

            memcpy(rx_buf.CanData,  can_pkg_tmp.data.CanData, can_pkg_tmp.len);
            id.CANx_ID = can_pkg_tmp.id.CANx_ID;
            rx_len = can_pkg_tmp.len;
            seg_polo = rx_buf.CanData_Struct.SegPolo;
            seg_num = rx_buf.CanData_Struct.SegNum;

            if(id.CanID_Struct.DestMACID == POWERBOARD_CAN_MAC_SRC_ID)
            {
                can_com_start_time = get_tick();
                if(rx_buf.CanData_Struct.SegPolo == ONLYONCE)
                {

                    //if((id.canx_id_t.source_id < SOURCE_ID_PREPARE_UPDATE) && (id.canx_id_t.source_id > SOURCE_ID_CHECK_TRANSMIT))
                    {
                        //process the data here//
                        tx_len = CmdProcessing(&id, rx_buf.CanData_Struct.Data, rx_len - 1, CanTxdataBuff);
                        //process the data here//
                        if(tx_len > 0)
                        {
                            Can1_TX(id.CANx_ID, CanTxdataBuff, tx_len);
                        }

                        //CanTX(MICO_CAN1, id.canx_id, test_data, sizeof(test_data));
                    }
                }
                else //long frame
                {
                    for(uint8_t i = 0; i < CAN_LONG_BUF_NUM; i++)
                    {
                        if(can_long_frame_buf->can_rcv_buf[i].used_len > 0)
                        {
                            if(get_tick() - can_long_frame_buf->can_rcv_buf[i].start_time > CAN_LONG_FRAME_TIME_OUT)
                            {
                                can_long_frame_buf->FreeBuf(i);
                                printf("LONG FRAME RCV TIMEOUT! ! ! !\r\n");
                            }
                        }
                    }

                    if(seg_polo == BEGIN)
                    {
                        buf_index = can_long_frame_buf->GetTheBufById(id.CANx_ID);
                        if(buf_index == CAN_BUF_NO_THIS_ID)
                        {
                            buf_index = can_long_frame_buf->GetOneFreeBuf();
                        }
                        else
                        {
                            //
                        }

                        if((buf_index == CAN_LONG_BUF_FULL) || (buf_index >= CAN_LONG_BUF_NUM))
                        {
                            printf("LONG FRAME RCV BUF IS FULL! ! ! !\r\n");
                            goto exit;
                        }
                        memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[0], rx_buf.CanData_Struct.Data, CAN_ONE_FRAME_DATA_LENTH);
                        can_long_frame_buf->can_rcv_buf[buf_index].used_len = CAN_ONE_FRAME_DATA_LENTH;
                        can_long_frame_buf->can_rcv_buf[buf_index].can_id = id.CANx_ID;
                        can_long_frame_buf->can_rcv_buf[buf_index].start_time = get_tick();
                    }
                    else if((seg_polo == TRANSING) || (seg_polo == END))
                    {
                        buf_index = can_long_frame_buf->GetTheBufById(id.CANx_ID);
                        if((buf_index == CAN_BUF_NO_THIS_ID) || (buf_index >= CAN_LONG_BUF_NUM))
                        {
                            printf("ERROR ! !\r\n");
                            goto exit;
                        }
                        can_long_frame_buf->can_rcv_buf[buf_index].start_time = get_tick();
                        if(seg_polo == TRANSING)
                        {
                            memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[seg_num * CAN_ONE_FRAME_DATA_LENTH], rx_buf.CanData_Struct.Data, CAN_ONE_FRAME_DATA_LENTH);
                            can_long_frame_buf->can_rcv_buf[buf_index].used_len += CAN_ONE_FRAME_DATA_LENTH;
                        }
                        if(seg_polo == END)
                        {
                            memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[seg_num * CAN_ONE_FRAME_DATA_LENTH], rx_buf.CanData_Struct.Data, rx_len - 1);
                            can_long_frame_buf->can_rcv_buf[buf_index].used_len += rx_len - 1;

                            printf("long frame receive complete\r\n");
                            for(uint8_t j = 0; j < can_long_frame_buf->can_rcv_buf[buf_index].used_len; j++)
                            {
                                printf("data[%d]: %d\r\n", j, can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[j]);
                            }
                            //process the data here//
                            /**********************/
                            //process the data here//

                            Can1_TX(id.CANx_ID, can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf, can_long_frame_buf->can_rcv_buf[buf_index].used_len);  // test :send the data back;
                            can_long_frame_buf->FreeBuf(buf_index);
                        }
                    }
                }

            }
        }
        else
        {
            delay_ms(20);
        }

    }
    
exit:
    return;
}



