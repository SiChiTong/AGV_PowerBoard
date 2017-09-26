/* 
*  Author: Adam Huang
*  Date:2017/01/23
*/

#include "can_protocol.h"
#include "protocol.h"
#include "platform.h"
#include "app_platform.h"
#include "stm32f1xx.h"
//#include "serial_leds.h"
#include "stm32f1xx_powerboard.h"
#include "Debug.h"


#include "can_fifo.h"

#define CanProtocolLog(format, ...)  custom_log("can protocol", format, ##__VA_ARGS__)

__IO uint32_t flag = 0xff;
//CanTxMsgTypeDef TxMessage;
CanRxMsgTypeDef RxMessage;
CanTxMsgTypeDef TxMessage;

extern platform_can_driver_t  platform_can_drivers[];
uint8_t CanTxdataBuff[CAN_LONG_FRAME_LENTH_MAX] = {0};

uint8_t swVersion[] = SW_VERSION;
uint8_t hwVersion[] = HW_VERSION;

CAN_TXDATA_STRUCT  CommandProcessing( uint32_t func_ID, uint8_t* dstPtr, uint8_t* pdata, uint32_t len );

can_fifo_t can_fifo_ram;
can_fifo_t *can_fifo = &can_fifo_ram;

can_pkg_t can_pkg[CAN_FIFO_SIZE] = {0};

 
typedef void (*CallBackFunc)(void);

typedef struct 
{
	uint32_t val;
	CallBackFunc callback;	
}CALLBACK_T;

const CALLBACK_T ID_table[]=
{
	{0,NULL},
	{1,NULL},
};

CALLBACK_T FuncId_table[] = {
	{0x00,NULL},
};


/**
  * @brief  rx msg handle
  * @param   
  * @retval 
	* @RevisionHistory
  */
CALLBACK_T* FuncIdHandle(uint32_t funcid)
{
    int i;
    int func_num = sizeof(FuncId_table)/sizeof(FuncId_table[0]);
    for(i = 0;i <func_num;i++)
    {
      if(FuncId_table[i].val == funcid)
      {
        return (&FuncId_table[i]);
      }
    }
    return NULL;
}

#define ONLYONCE       0x00
#define BEGIN         0x01
#define TRANSING       0x02
#define END            0x03


void CanTX(mico_can_t can_type, uint32_t CANx_ID,uint8_t* pdata,uint16_t len)
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
	TxMessage.IDE   = CAN_ID_EXT;					 //扩展模式
	TxMessage.RTR   = CAN_RTR_DATA;				 //发送的是数据
	//if(roundCount <= 1)
    if(t_len <= 7)
    {
        TxMsg.CanData_Struct.SegPolo = ONLYONCE;
        TxMessage.DLC = t_len+1;		
        
        
        memcpy(&TxMessage.Data[1],pdata,t_len);
        TxMessage.Data[0] = TxMsg.CanData[0];
        
        if((CAN_USED->TSR&0x1C000000))
        {
            MicoCanMessageSend(MICO_CAN1, &TxMessage);//
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
                    MicoCanMessageSend(MICO_CAN1, &TxMessage);//发送报文	
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
                        MicoCanMessageSend(MICO_CAN1, &TxMessage);//
                    }
                }
            }
            
        }
        
	}
}

/*******************************ID HANDLE END*************************************************/




//////  function id define  //////
#define CAN_FUN_ID_RESET        0x06
#define CAN_FUN_ID_WRITE        0x01
#define CAN_FUN_ID_READ         0x02
#define CAN_FUN_ID_TRIGGER      0x03


//////  source id define  //////
#define CAN_SOURCE_ID_READ_VERSION      0x01
#define CAN_SOURCE_ID_MODULE_STATE      0x81
#define CAN_SOURCE_ID_ERROR_STATE       0x82
#define CAN_SOURCE_ID_READ_ADC_DATA     0x83
#define CAN_SOURCE_ID_READ_RK_STATE     0x84
#define CAN_SOURCE_ID_PWM_LED           0x85        


#define CAN_READ_DATA               0x80

void UploadAdcData(void)
{
    CAN_ID_UNION id;
    id.CanID_Struct.ACK = 1;
    id.CanID_Struct.DestMACID = 0;////
    id.CanID_Struct.FUNC_ID = CAN_FUN_ID_TRIGGER;
    id.CanID_Struct.SourceID = CAN_SOURCE_ID_READ_ADC_DATA;
    id.CanID_Struct.SrcMACID = CAN_NOAH_PB_ID;////

    CanTX( MICO_CAN1, id.CANx_ID, (uint8_t *)voltageConvert, sizeof(voltageData_t) );
}

#define CMD_NOT_FOUND   0
uint16_t CmdProcessing(CAN_ID_UNION *id, const uint8_t *data_in, const uint16_t data_in_len, uint8_t *data_out)
{
    //uint8_t data_out_len;
    id->CanID_Struct.ACK = 1;   
    id->CanID_Struct.DestMACID = id->CanID_Struct.SrcMACID;
    id->CanID_Struct.SrcMACID = CAN_NOAH_PB_ID;
    id->CanID_Struct.res = 0;
    //id->CanID_Struct.FUNC_ID = 
    switch(id->CanID_Struct.FUNC_ID)
    {
        case CAN_FUN_ID_RESET:
            platform_mcu_reset();
            break;
        case CAN_FUN_ID_WRITE:
        case CAN_FUN_ID_READ:
            switch(id->CanID_Struct.SourceID)
            {
                case CAN_SOURCE_ID_READ_VERSION:
                    data_out[0] = data_in[0];
                    if(data_in[0] == 1)//read software version
                    {
                        memcpy(&data_out[1], SW_VERSION, sizeof(SW_VERSION));
                        //return strlen(SW_VERSION) + 1;
                        return sizeof(SW_VERSION) + 1;
                    }
                    else if(data_in[0] == 2)//protocol version
                    {
                        memcpy(&data_out[1], PROTOCOL_VERSION, sizeof(PROTOCOL_VERSION));
                        return sizeof(PROTOCOL_VERSION) +1;
                        
                    }
                    else if(data_in[0] == 3)//hardware version
                    {
                        memcpy(&data_out[1], HW_VERSION, sizeof(HW_VERSION));
                        return sizeof(HW_VERSION) + 1;
                    }
                    return CMD_NOT_FOUND;
                    break;
                case CAN_READ_DATA:
                
                    break;
                case CAN_SOURCE_ID_MODULE_STATE:
#if 0
                    data_out[0] = data_in[0];
                    if(data_in[0] == 1)//write
                    {
                        if(data_in[1] == 1)//group num = 1
                        {
                            uint32_t module = (data_in[5]) | (data_in[4]<<8) | (data_in[3] << 16) | (data_in[2] << 24);
                            module &= 0xffffffff;
                            BSP_Power_OnOff((PowerEnable_TypeDef)module,(PowerOnOff_TypeDef)data_in[6]);
                            uint32_t tmp = GetModulePowerState(POWER_ALL);
                            data_out[1] = data_in[1];
                            memcpy( &data_out[2],(uint8_t *)(&tmp) , 4 );
                            return 6;
                        }   
                        if(data_in[1] == 2)//group num = 2
                        {

                            uint32_t module = (data_in[5]) | (data_in[4]<<8) | (data_in[3] << 16) | (data_in[2] << 24);
                            module &= 0xffffffff;
                            BSP_Power_OnOffEx((PowerEnable_2_TypeDef)module,(PowerOnOff_TypeDef)data_in[6]);
                            uint32_t tmp = GetModulePowerStateEx(POWER_ALL_2);
                            data_out[1] = data_in[1];
                            memcpy( &data_out[2],(uint8_t *)(&tmp) , 4 );
                            return 6;
                        }
                    }
                    else if(data_in[0] == 2)//read
                    {
                        
                        if(data_in[1] == 1)//group num = 1
                        {
                            uint32_t tmp = GetModulePowerState(POWER_ALL);
                            data_out[1] = 1;
                            memcpy( &data_out[2],(uint8_t *)(&tmp) , 4 );
                            
                            return 6;
                        }
                        if(data_in[1] == 2)//group num = 2
                        {
                            uint32_t tmp = GetModulePowerStateEx(POWER_ALL_2);
                            data_out[1] = 2;
                            memcpy( &data_out[2],(uint8_t *)(&tmp) , 4 );
                            
                            return 6;
                        }               
                    }
                    return CMD_NOT_FOUND;
#endif
                    break;
                case CAN_SOURCE_ID_ERROR_STATE: 
                    memcpy( data_out, voltageConvertData->faultBitTemp, 5 );
                    return 5;
                    
                case CAN_SOURCE_ID_READ_ADC_DATA:  
                    voltageDebug.isNeedUpload = YES;
                    voltageDebug.uploadRate = data_in[0];
                    return 0;
                case CAN_SOURCE_ID_READ_RK_STATE:
#if 0
                  #define DATA_NONE       0xff
                    if(data_in[0] == 1)//read
                    {
                        data_out[1] = GetRkState(POWER_HEAD_RK);
                        data_out[2] = GetRkState(POWER_CHEST_RK);
                    }
                    if(data_in[0] == 2)//write
                    {
                        if(data_in[1] != DATA_NONE)
                        {
                            data_out[1] = SetRkState(POWER_HEAD_RK, data_in[1]);
                        }
                        if(data_in[2] != DATA_NONE)
                        {
                            data_out[2] = SetRkState(POWER_CHEST_RK, data_in[2]);
                        }       
                    }
                     data_out[0] = data_in[0];
                     return 3;
#endif
                     break;
                case CAN_SOURCE_ID_PWM_LED:
#if 0
                    #define DATA_NONE       0xff
                    data_out[0] = data_in[0];
                    if(data_in[0] == 1)//read brightness
                    {
                        
                        if(data_in[1] != DATA_NONE)
                        {
                            data_out[1] = GetLedBrightness(LED1);
                        }
                        else
                        {
                            data_out[1] = DATA_NONE;
                        }   
                        
                        if(data_in[2] != DATA_NONE)
                        {
                            data_out[2] = GetLedBrightness(LED2);
                        }
                        else
                        {
                            data_out[2] = DATA_NONE;
                        }
                        
                    }
                    if(data_in[0] == 2)//write
                    {
                        if(data_in[1] != DATA_NONE)
                        {
                            data_out[1] = SetLedBrightness(LED1,data_in[1]);
                        }
                        else
                        {
                            data_out[1] = DATA_NONE;
                        }
                        
                        
                        if(data_in[2] != DATA_NONE)
                        {
                            data_out[2] = SetLedBrightness(LED2,data_in[2]);
                        }
                        else
                        {
                            data_out[2] = DATA_NONE;
                        }
                    }
                    return 3;
#endif
                    break;
                default :
                    break;
            }


        default: 
        break;
    }

    return CMD_NOT_FOUND;
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
    
    CanFifoInit(can_fifo, can_pkg, CAN_FIFO_SIZE);
}

#define CAN_LONG_FRAME_TIME_OUT     5000/SYSTICK_PERIOD

#define CAN_COMM_TIME_OUT           5000
uint32_t can_comm_start_time;


void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
  CanProtocolLog("can err callback");
}



#define CAN_LONG_FRAME_TIME_OUT     5000/SYSTICK_PERIOD

#define CAN_COMM_TIME_OUT           5000
uint32_t can_comm_start_time;


void can_protocol_period( void )
{   
    static CAN_ID_UNION id;
    uint16_t tx_len;
    uint8_t rx_len;
    static CAN_DATA_UNION rx_buf;
    can_pkg_t can_pkg_tmp;
    uint8_t buf_index;
    uint8_t seg_polo;
    uint8_t seg_num;
    uint8_t test_data[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    while(IsCanFifoEmpty(can_fifo) == FALSE)
    {
        CanFifoGetCanPkg(can_fifo, &can_pkg_tmp); 
      
        memcpy(rx_buf.CanData,  can_pkg_tmp.data.CanData, can_pkg_tmp.len);
        id.CANx_ID = can_pkg_tmp.id.CANx_ID;
        rx_len = can_pkg_tmp.len;
        seg_polo = rx_buf.CanData_Struct.SegPolo;
        seg_num = rx_buf.CanData_Struct.SegNum;
        
        if(rx_buf.CanData_Struct.SegPolo == ONLYONCE)
        {
            
            //if( (id.CanID_Struct.SourceID < SOURCE_ID_PREPARE_UPDATE) && (id.CanID_Struct.SourceID > SOURCE_ID_CHECK_TRANSMIT) )
            {
                    //process the data here//
                    tx_len = CmdProcessing(&id, rx_buf.CanData_Struct.Data, rx_len - 1, CanTxdataBuff );
                    //process the data here//
                    if(tx_len > 0)
                    {
                        //CanTX( MICO_CAN1, id.CANx_ID, CanTxdataBuff, tx_len );
                    }
                    
                    CanTX( MICO_CAN1, id.CANx_ID, test_data, sizeof(test_data) );
            }
        }
        else //long frame
        {
            for(uint8_t i = 0; i < CAN_LONG_BUF_NUM; i++)
            {
                if(can_long_frame_buf->can_rcv_buf[i].used_len > 0)
                {
                    if(os_get_time() - can_long_frame_buf->can_rcv_buf[i].start_time > CAN_LONG_FRAME_TIME_OUT)
                    {
                        can_long_frame_buf->FreeBuf(i);
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
                
                if( (buf_index == CAN_LONG_BUF_FULL) || (buf_index >= CAN_LONG_BUF_NUM) )
                {
                    CanProtocolLog("LONG FRAME RCV BUF IS FULL! ! ! !\r\n");              
                    goto exit;
                }
                memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[0], rx_buf.CanData_Struct.Data, CAN_ONE_FRAME_DATA_LENTH);
                can_long_frame_buf->can_rcv_buf[buf_index].used_len = CAN_ONE_FRAME_DATA_LENTH;
                can_long_frame_buf->can_rcv_buf[buf_index].can_id = id.CANx_ID;
                can_long_frame_buf->can_rcv_buf[buf_index].start_time = os_get_time();
            }
            else if((seg_polo == TRANSING) || (seg_polo == END))
            {
                buf_index = can_long_frame_buf->GetTheBufById(id.CANx_ID);
                if((buf_index == CAN_BUF_NO_THIS_ID) || (buf_index >= CAN_LONG_BUF_NUM))
                {
                    CanProtocolLog("ERROR ! !\r\n");
                    goto exit;
                }
                can_long_frame_buf->can_rcv_buf[buf_index].start_time = os_get_time();
                if(seg_polo == TRANSING)
                {
                    memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[seg_num*CAN_ONE_FRAME_DATA_LENTH], rx_buf.CanData_Struct.Data, CAN_ONE_FRAME_DATA_LENTH);
                    can_long_frame_buf->can_rcv_buf[buf_index].used_len += CAN_ONE_FRAME_DATA_LENTH;
                }
                if(seg_polo == END)
                {
                    memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[seg_num*CAN_ONE_FRAME_DATA_LENTH], rx_buf.CanData_Struct.Data, rx_len - 1);
                    can_long_frame_buf->can_rcv_buf[buf_index].used_len += rx_len - 1;
                    
                    printf("long frame receive complete\r\n");
                    for(uint8_t j = 0; j < can_long_frame_buf->can_rcv_buf[buf_index].used_len; j++)
                    {
                      printf("data[%d]: %d\r\n",j,can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[j]);
                    }
                    //process the data here//
                    /**********************/
                    //process the data here//
                    
                    CanTX( MICO_CAN1, id.CANx_ID, can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf, can_long_frame_buf->can_rcv_buf[buf_index].used_len);  // test :send the data back;             
                    can_long_frame_buf->FreeBuf(buf_index);
                }       
            }
        }
    }
    //MicoCanMessageRead( MICO_CAN1, &RxMessage);

    
exit:    
    //platform_can_drivers[MICO_CAN1].rx_complete = 0;
    
    return;
}







