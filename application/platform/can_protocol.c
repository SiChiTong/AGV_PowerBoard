/* 
*  Author: Adam Huang
*  Date:2017/01/23
*/

#include "can_protocol.h"
#include "protocol.h"
#include "platform.h"
#include "stm32f1xx.h"
#include "serial_leds.h"
#include "stm32f1xx_powerboard.h"
#include "Debug.h"

#define DESTID          0x02
#define SRCID           0x06

__IO uint32_t flag = 0xff;
//CanTxMsgTypeDef TxMessage;
CanRxMsgTypeDef RxMessage;
extern platform_can_driver_t  platform_can_drivers[];
uint8_t CanUpdataBuff[64];
uint8_t CanRxdataBuff[64];
uint8_t CanTxdataBuff[64] = {0};
uint8_t swVersion[] = SW_VERSION;
uint8_t hwVersion[] = HW_VERSION;

CAN_TXDATA_STRUCT  CommandProcessing( uint32_t func_ID, uint8_t* dstPtr, uint8_t* pdata, uint32_t len );
//CAN_TXDATA_STRUCT FirmwareUpgrade(uint32_t ID,uint8_t* pdata,uint32_t len);

/***************************ID HANDLE BEGIN*****************************************************/
/**
  * @brief  
  * @param   
  * @retval 
	* @RevisionHistory
  */

 
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

//static CM_CAN_ID_T* CanId;	
/**
  * @brief  prase rx msg id
* @param   id: read from reg
  * @retval 
	* @RevisionHistory
  */
CM_CAN_ID_T RxIdPrase(uint32_t id)
{
    CM_CAN_ID_T CanIdRx;	
    CanIdRx.FrameType = ((id & FrameTypeMask)>>FrameTypeBit);
    CanIdRx.SrcId = ((id & SrcIdMask)>>SrcIdBit);
    CanIdRx.DestId = ((id & DestIdMask)>>DestIdBit);
    if(CanIdRx.FrameType)
    {
      CanIdRx.CanID_U.Ack = id & AckMask_U;
      CanIdRx.CanID_U.ResendFlag = id & ResendFlagMask_U;
      CanIdRx.CanID_U.SegmentNum = id & SegmentNumMask_U;
    }
    else
    {
      CanIdRx.CanID_D.Property = ((id & PropertyMask_D) >> PropertyBit_D);
      CanIdRx.CanID_D.FuncId = ((id & FuncIdMask_D)>>FuncIdBit_D);
      CanIdRx.CanID_D.SegmentNum = ((id & SegmentNumMask_D)>>SegmentNumBit_D);
      CanIdRx.CanID_D.Ack = ((id & AckMask_D)>>AckBit_D);
      CanIdRx.CanID_D.EndFlag = ((id & EndFlagMask_D)>>EndFlagBit_D);
      CanIdRx.CanID_D.ResendFlag = ((id & ResendFlagMask_D)>>ResendFlagBit_D);
    }
    return CanIdRx;
}
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

uint32_t TxIdBuild(CM_CAN_ID_T* CanId)
{	
    uint32_t id;
    CM_CAN_ID_T* TxID;
    TxID = CanId;

    if(TxID->FrameType)	// normal
    {
      id = (TxID->FrameType<<FrameTypeBit)|(TxID->SrcId<<SrcIdBit)|(TxID->DestId<<DestIdBit)|(TxID->CanID_U.Ack<<AckBit_U)\
                            |(TxID->CanID_U.ResendFlag<<ResendFlagBit_U)|(TxID->CanID_U.SegmentNum<<SegmentNumBit_U);
    }	
    else // update
    {
      id = (TxID->FrameType<<FrameTypeBit)|(TxID->SrcId<<SrcIdBit)|(TxID->DestId<<DestIdBit)|(TxID->CanID_D.Property<<PropertyBit_D)|(TxID->CanID_D.FuncId<<FuncIdBit_D)\
                            |(TxID->CanID_D.SegmentNum<<SegmentNumBit_D)|(TxID->CanID_D.Ack<<AckBit_D)|(TxID->CanID_D.EndFlag<<EndFlagBit_D)|(TxID->CanID_D.ResendFlag<<ResendFlagBit_D);
    }
    return id;
}


void CM_CAN_Tx( mico_can_t can_type, CM_CAN_ID_T id, uint8_t* pdata, uint16_t len )
{
    uint8_t t_len;
//    uint32_t type;
//    uint32_t SrcId;
//    uint32_t DestId;
//    uint32_t ack;
//    uint32_t EndFlag;
    uint32_t SegmentNum;
    
    CM_CAN_ID_T TxData;
    
    t_len = len;
//    ack = 1;
//    SrcId = CAN_LOCAL_ID;
//    DestId = DESTID;
    SegmentNum = 0;
    CanTxMsgTypeDef *TxMessage = platform_can_drivers[can_type].handle->pTxMsg;
    TxData.FrameType = id.FrameType;
    TxData.DestId = id.SrcId;
    TxData.SrcId = id.DestId;
    
    if( TxData.FrameType == EXTID_FRAME_TYPE_DATA )// normal
    {
        TxData.CanID_D.Ack = 1 ;
        TxData.CanID_D.EndFlag = 1;
        TxData.CanID_D.FuncId = id.CanID_D.FuncId;
        TxData.CanID_D.ID = id.CanID_D.ID;
        TxData.CanID_D.Property = id.CanID_D.Property;
        TxData.CanID_D.ResendFlag = id.CanID_D.ResendFlag;
        TxData.CanID_D.SegmentNum = 0;
        //size is longer than 8
        if( t_len > 8 )
        {
          for( SegmentNum = 0; SegmentNum < t_len/8; SegmentNum++ )
          {
            TxMessage->DLC = 8;					
            TxData.CanID_D.SegmentNum = SegmentNum;
            //memcpy( TxMessage->Data, &pdata[SegmentNum * 8], 8 );
            
            if( t_len%8 == 0) 
            {
              if( SegmentNum + 1 < t_len/8 )
              {
                 TxData.CanID_D.EndFlag = 0;
              }
              else
              {
                 TxData.CanID_D.EndFlag = 1;
              }
            }

            TxMessage->ExtId = TxIdBuild( &TxData );
            TxMessage->IDE = CAN_ID_EXT;	
            TxMessage->RTR = CAN_RTR_DATA;
            
            //check is there available mail for use
            if( (CAN1->TSR&0x1C000000) )
            {			
               MicoCanMessageSend( can_type, &pdata[SegmentNum * 8], 8 );
            }
          }
          if( t_len%8 != 0 )
          {
            TxMessage->DLC = t_len%8;			
            TxData.CanID_D.EndFlag = 1;
            TxData.CanID_D.SegmentNum = SegmentNum+1;
            //memcpy( TxMessage->Data, &pdata[SegmentNum*8], t_len%8 );
            
            TxMessage->ExtId=TxIdBuild(&TxData);
            TxMessage->IDE=CAN_ID_EXT;
            TxMessage->RTR=CAN_RTR_DATA;
            
            if((CAN1->TSR&0x1C000000))
            {
              MicoCanMessageSend( can_type, &pdata[SegmentNum * 8], t_len%8 );
            }
          }
                
        }
        else
        {
          TxData.CanID_D.EndFlag = 1;
          TxData.CanID_D.SegmentNum = 1;
  
          TxMessage->DLC=t_len;	
          //memcpy(TxMessage->Data,pdata,t_len);
  
          TxMessage->ExtId=TxIdBuild(&TxData);
          TxMessage->IDE=CAN_ID_EXT;
          TxMessage->RTR=CAN_RTR_DATA;
  
          if((CAN1->TSR&0x1C000000))
          {
            MicoCanMessageSend( can_type, pdata, t_len );
          }	
        }	
    }
    else // update
    {
        TxData.CanID_U.Ack = 1;
        TxData.CanID_U.ID = id.CanID_U.ID;
        TxData.CanID_U.ResendFlag = id.CanID_U.ResendFlag;
        TxData.CanID_U.SegmentNum = 0;
                //size is longer than 8
        if(t_len>8)
        {
          for(SegmentNum = 0 ;SegmentNum < t_len/8 ;SegmentNum++)
          {
            TxMessage->DLC=8;		
            TxData.CanID_U.SegmentNum = SegmentNum;
            //memcpy(TxMessage->Data,&pdata[SegmentNum*8],8);

            TxMessage->ExtId=TxIdBuild(&TxData);
            TxMessage->IDE=CAN_ID_EXT;
            TxMessage->RTR=CAN_RTR_DATA;
            
            //check is there available mail for use
            if((CAN1->TSR&0x1C000000))
            {			
              MicoCanMessageSend( can_type, &pdata[SegmentNum*8], 8 );
            }
          }
          if(t_len%8 != 0)
          {
            TxMessage->DLC=t_len%8;			
            TxData.CanID_U.SegmentNum = SegmentNum+1;
            //memcpy(TxMessage->Data,&pdata[SegmentNum*8],t_len%8);
            
            TxMessage->ExtId=TxIdBuild( &TxData );
            TxMessage->IDE=CAN_ID_EXT;
            TxMessage->RTR=CAN_RTR_DATA;
            
            if( (CAN1->TSR&0x1C000000) )
            {
               MicoCanMessageSend( can_type, &pdata[SegmentNum*8], t_len%8 );
            }
          }
        }
        else
        {
          TxData.CanID_U.SegmentNum = 1;
  
          TxMessage->DLC=t_len;	
          memcpy(TxMessage->Data,pdata,t_len);
  
          TxMessage->ExtId=TxIdBuild(&TxData);
          TxMessage->IDE=CAN_ID_EXT;
          TxMessage->RTR=CAN_RTR_DATA;
  
          if((CAN1->TSR&0x1C000000))
          {
             MicoCanMessageSend( can_type, pdata, t_len );
          }	
        }	
    }
}
#if 0
CAN_TXDATA_STRUCT FirmwareUpgrade(uint32_t ID,uint8_t* pdata,uint32_t len)
{
	uint8_t PageNum=0;
	int DataNum=0; 
	uint8_t DeltAdd=0;
	uint8_t MD5Check[16]={0};
	int DataSize=0;
	FLASH_Status FlashEraseState; 
	FLASH_Status FlashWriteState; 
	CAN_TXDATA_STRUCT CanData;
	uint8_t EraseState=0;
	uint8_t WriteState=0;
	uint8_t rx_id;
	uint8_t rx_data[16];
	uint8_t tx_data[32];
	uint8_t rx_len,tx_len;
	uint8_t *ptx_data;
	int i;
	
	Md5Context MD5Ctx;
	MD5_HASH md5_ret;
	
	rx_id = ID;
	rx_len = 0 ;
	memcpy(rx_data,pdata,rx_len);
	//Éý¼¶×¼±¸
	switch(rx_id)
	{
		case 0x00:
				EraseState=0;
				DataNum=0;
				memcpy(MD5Check,&rx_data,16);
				DataSize = (rx_data[3]<<24)+(rx_data[2]<<16)+(rx_data[1]<<8)+rx_data[0];
				//³¬¹ý´óÐ¡
				if(DataSize>128*1024)
				{
					EraseState |= 0x01;
				}
				else
				{
					EraseState &= 0x0E;
					FLASH_Unlock();
					RCC_HSICmd(ENABLE);																									
					FLASH_SetLatency(FLASH_Latency_2);
					FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

					for(PageNum=0;PageNum<0x40;PageNum++)
					{
						IWDG_ReloadCounter();
						
						FlashEraseState=FLASH_ErasePage(UPDATEADDRESS+PageNum*0x800);
						if(FlashEraseState!=4)
						{
							EraseState =0x02;
							break;
						}
						else
						{
							EraseState &=0x0D;
						}
					}				
					RCC_HSICmd(DISABLE);	
				}
				tx_data[0] = ID;
				tx_data[1] = EraseState;	
				CanData.len = 2;
				ptx_data = tx_data;

				break;
	
		//Éý¼¶Êý¾Ý°ü
		case 0x01:
				WriteState=0;
				DeltAdd = 8;	//TBD
				IWDG_ReloadCounter();															
			//½«µ±Ç°½ÓÊÕµÄÊý¾ÝÓëÉÏ´ÎµÄÊý¾Ý½øÐÐ±È½Ï
				if(memcmp(rx_data,RxMessage.Data,8))
				{									
					//°ë×ÖÐ´Èë
					for(i=0;i<DeltAdd;i+=2)
					{
						unsigned short tempData;														
						RCC_HSICmd(ENABLE);
						tempData = (rx_data[i]<<8)+rx_data[i+1];
						FlashWriteState=FLASH_ProgramHalfWord(UPDATEADDRESS+DataNum*2,tempData);																											
						if(FlashWriteState!= 4)
						{
							DataNum-=i/2;
							WriteState = 0x01;
							break;
						}
						else
						{
							DataNum ++;
							WriteState &= 0x0E;
						}
					}
					
					if(DeltAdd%2)
					{
						unsigned short tempData;
						tempData = (0xFF00)+rx_data[i*2+4];
						FlashWriteState=FLASH_ProgramHalfWord(UPDATEADDRESS+DataNum*2,tempData);	
						if(FlashWriteState!= 4)
						{
							DataNum-=i/2;
							WriteState |= 0x02;
							break;
						}
						else
						{
							DataNum ++;
							WriteState &= 0x0D;
						}
						RCC_HSICmd(DISABLE);
					}
				//½«Êý¾Ý±£´æÖÁ±äÁ¿ÓÃÓÚÓëÏÂ´Î½ÓÊÕµÄÊý¾Ý½øÐÐ±È½Ï
					if(WriteState==0)
					{
						memcpy(rx_data,RxMessage.Data,8);		
					}															
				}
				else
				{
					//×Ö·û´®ÖØ¸´
					 WriteState = 0x00;
				}
				tx_data[0] = ID;
				tx_data[1] = WriteState;
				CanData.len = 2;	
				ptx_data = tx_data;				
			break;
		//Éý¼¶Íê³É
		case 0x02:
				UpdateFlag=0;
				DataNum=0;
				WriteState=0;
				Md5Initialise(&MD5Ctx);
				Md5Update( &MD5Ctx, (uint8_t *)UPDATEADDRESS, DataSize);
				Md5Finalise( &MD5Ctx, &md5_ret );
				if(memcmp(md5_ret.bytes, MD5Check, 16)!=0)
				{
						WriteState |= 0x01;																											
				}							
				else
				{
					WriteState &=0x0E;
					bootTable.boot_table_t.length = DataSize;
					bootTable.boot_table_t.start_address = UPDATEADDRESS;
					bootTable.boot_table_t.type = 'A';
					bootTable.boot_table_t.upgrade_type = 'U';
					RCC_HSICmd(ENABLE);
					//²Á³ý²ÎÊýÒ³
					for(PageNum=0;PageNum<0x02;PageNum++)
					{
						IWDG_ReloadCounter();
						FlashWriteState=FLASH_ErasePage(PARAMSADDRESS+PageNum*0x800);
						if(FlashWriteState!= 4)
						{
							DataNum-=i/2;
							WriteState |= 0x02;
							break;
						}
						else
						{
							WriteState &= 0x0D;
						}
					}
					
					if(WriteState==0x00)
					{
						//Ð´²ÎÊýÇø
						for(i=0;i<16;i+=4)
						{
							unsigned int paraData;
							paraData = (bootTable.data[i+3]<<24)+(bootTable.data[i+2]<<16)+(bootTable.data[i+1]<<8)+bootTable.data[i];
							FlashWriteState=FLASH_ProgramWord(PARAMSADDRESS+DataNum*4,paraData);
							if(FlashWriteState!= 4)
							{
								DataNum-=i/2;
								WriteState |= 0x04;
								break;
							}
							else
							{
								DataNum ++;
								WriteState &= 0x0B;
							}
						}
						
						for(i=16;i<24;i+=4)
						{
							unsigned int paraData;
							paraData = (bootTable.data[i+3]<<24)+(bootTable.data[i+2]<<16)+(bootTable.data[i+1]<<8)+bootTable.data[i];
							FlashWriteState=FLASH_ProgramWord(PARAMSADDRESS+DataNum*4,paraData);
							if(FlashWriteState!= 4)
							{
								DataNum-=i/2;
								WriteState |= 0x08;
								break;
							}
							else
							{
								DataNum ++;
								WriteState &= 0x07;
							}
						}																
					}	
					RCC_HSICmd(DISABLE);																					
				}	
				
				tx_data[1] = WriteState;	
				tx_data[0] = 0x02;												
				FLASH_Lock();
				DataNum = 0;													
				CanData.len = 2;
				ptx_data = tx_data;
			break;
				
		default :
			  rx_id = 0xFF;
			  tx_len   = 0x06;
			  ptx_data = tx_data;
			break;
	}
	CanData.len = tx_len;
	CanData.FuncID = rx_id;
	CanData.pdata = ptx_data;
	return CanData;
}
#endif
/*******************************ID HANDLE END*************************************************/

CAN_TXDATA_STRUCT  CommandProcessing( uint32_t func_ID, uint8_t* dstPtr, uint8_t* pdata, uint32_t len )
{
    CAN_TXDATA_STRUCT CanData;
    
//    CM_CAN_ID_T TxData;
    uint8_t rx_id;
    uint8_t rx_data[8];
    uint8_t* tx_data;
    uint8_t rx_len,tx_len;
    uint8_t *ptx_data;
    
    rx_id = func_ID;
    rx_len = len ;
    
    tx_data = dstPtr;
    memcpy( rx_data, pdata, rx_len );
    
    switch( rx_id )
    {
        case CAN_CMD_READ_VERSION:
          
          memcpy( tx_data, hwVersion, 2 );
          memcpy( &tx_data[4], swVersion, 11 );
          tx_len = 1;
          ptx_data = tx_data;
          break;
        case CAN_CMD_LEDS_CONTROL:
          *tx_data++ = serial_leds->modeType;
          *tx_data++ = (serial_leds->effectType & 0xff00) >> 8;
          *tx_data++ = (serial_leds->effectType & 0x00ff);
          tx_len = 3;
          break;
        default :
          break;
    }
  
    CanData.FuncID = rx_id ;
    CanData.len    = tx_len;
    CanData.pdata  = ptx_data;

    return CanData;
}

static uint8_t CANUpDataLen = 0;
//static uint8_t CANRxDataLen = 0;

void can_protocol_period( void )
{
    if( platform_can_drivers[MICO_CAN1].rx_complete == 0 )
    {
      return;
    }
    
    CM_CAN_ID_T CanData ;
    CAN_TXDATA_STRUCT TxCanData;
    
    
    memcpy( &RxMessage, platform_can_drivers[MICO_CAN1].handle->pRxMsg, sizeof(CanRxMsgTypeDef) );
    CanData = RxIdPrase( RxMessage.ExtId );

    if( CanData.FrameType == 0 )
    {
        if( CanData.CanID_D.EndFlag == 0 )
        {
//            CANRxDataLen = CanData.CanID_D.SegmentNum * 8;
            memcpy( &CanRxdataBuff[CanData.CanID_D.SegmentNum*8], RxMessage.Data, RxMessage.DLC );
        }
        else
        {
//            CANRxDataLen = CanData.CanID_D.SegmentNum*8 + RxMessage.DLC;
            memcpy( &CanRxdataBuff[CanData.CanID_D.SegmentNum*8], RxMessage.Data, RxMessage.DLC );		
    
            TxCanData = CommandProcessing( CanData.CanID_D.FuncId, CanTxdataBuff, CanRxdataBuff, RxMessage.DLC );
            CM_CAN_Tx( MICO_CAN1, CanData, TxCanData.pdata, TxCanData.len );
//            CANRxDataLen = 0;
        }
    }
#if 0
    else if( CanData.FrameType == 1 )
    {
        if( CanData.CanID_U.SegmentNum <= 10 )//update prepare
        {
            if( CanData.CanID_U.ID != 0 )//muti data process
            {
                CANUpDataLen = CanData.CanID_U.SegmentNum*8;
                memcpy( &CanUpdataBuff[CanData.CanID_U.SegmentNum*8], RxMessage.Data, RxMessage.DLC );
            }
            else
            {
                CANUpDataLen = CanData.CanID_U.SegmentNum*8 + RxMessage.DLC;
                memcpy( &CanUpdataBuff[CanData.CanID_U.SegmentNum*8], RxMessage.Data, RxMessage.DLC );				
                //update cmd process
                TxCanData = FirmwareUpgrade( CanData.CanID_U.ID, CanUpdataBuff, CANUpDataLen );
                CM_CAN_Tx( MICO_CAN1, CanData, TxCanData.pdata, TxCanData.len );
                CANUpDataLen = 0;
            }
        }
        else//update data package
        {
            TxCanData = FirmwareUpgrade( CanData.CanID_U.ID, CanUpdataBuff, CANUpDataLen );
            CM_CAN_Tx( MICO_CAN1, CanData, TxCanData.pdata, TxCanData.len );
        }
    }
#endif
    platform_can_drivers[MICO_CAN1].rx_complete = 0;
}






