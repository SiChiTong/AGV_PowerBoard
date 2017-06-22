/* 
*  Author: Adam Huang
*  Date:2017/01/23
*/

#ifndef __CAN_PROTOOCOL_H

#include "mico.h"

#define CAN_ID          (0x434D0000)      //CM
#define CAN_LOCAL_ID    (0x06)            //local ID


#define CAN_CMD_READ_VERSION            0x01
#define CAN_CMD_LEDS_CONTROL            0x20
#define CAN_CMD_S_SYS_V_BAT             0x21
#define CAN_CMD_
typedef union
{
	 struct{
		uint32_t start_address; // the address of the bin saved on flash.
		uint32_t length; // file real length
		uint8_t version[8];
		uint8_t type; // B:bootloader, P:boot_table, A:application, 
		uint8_t upgrade_type; //u:upgrade, 
		uint8_t reserved[6];
	}boot_table_t;
 char data[24];
 }BOOTLOADER_UNION;


typedef struct
{
	uint8_t FuncID;
	uint16_t len;
	uint8_t *pdata;
}CAN_TXDATA_STRUCT;

typedef struct {
	uint32_t ID;
	uint32_t TimeStamp;
	uint8_t TimeFlag;
	uint8_t SendType;
	uint8_t RemoteFlag;
	uint8_t ExternFlag;
	uint8_t DataLen;
	uint8_t Data[8];
	uint8_t Reserved[3];

} CM_CAN_T;

typedef struct {
	uint32_t AccCode;
	uint32_t AccMask;
	uint32_t Filter;
	uint32_t Bundrate;
	uint8_t Mode;
	
} CM_CAN_CONFIG_T;

typedef enum 
{
	 FrameTypeBit = 28,
	 SrcIdBit = 27,
	 DestIdBit = 16,
	 AckBit_U = 15,
	/*固件升级MASK*/
	 ResendFlagBit_U= 14,
         SegmentNumBit_U = 13,
	/*控制指令MASK*/
	 PropertyBit_D = 14,
	 FuncIdBit_D = 7,
	 SegmentNumBit_D = 3,
	 AckBit_D = 2,
	 EndFlagBit_D = 1,
	 ResendFlagBit_D = 0,
} CM_CAN_ID_BIT;	

typedef enum 
{
	 FrameTypeMask = 0x10000000,
	 SrcIdMask = 0x0fc00000,
	 DestIdMask = 0x003f0000,
	 AckMask_U = 0x00008000,
	/*固件升级MASK*/
	 ResendFlagMask_U= 0x00004000,
	 SegmentNumMask_U = 0x00003fff,
	/*控制指令MASK*/
	 PropertyMask_D = 0x0000c000,
	 FuncIdMask_D = 0x00003f80,
	 SegmentNumMask_D = 0x00000078,
	 AckMask_D = 0x00000004,
	 EndFlagMask_D = 0x00000002,
	 ResendFlagMask_D = 0x00000001,
} CM_CAN_ID_MASK;

typedef struct
{
	uint32_t ID;
	uint8_t FrameMode;
	uint8_t FrameType;
	uint8_t DataLen;
	uint8_t Data[8];	
}CM_CAN_FRAME_T;

typedef struct 
{
	uint32_t Ack;
	uint32_t ResendFlag;
	uint32_t SegmentNum;
	uint32_t ID;
} CM_CAN_ID_U_T;

typedef struct 
{
	uint32_t Property;
	uint32_t FuncId;
	uint32_t SegmentNum;
	uint32_t Ack;
	uint32_t EndFlag;
	uint32_t ResendFlag;
	uint32_t ID;
} CM_CAN_ID_D_T;

typedef struct
{
	uint32_t FrameType;
	uint32_t SrcId;
	uint32_t DestId;
	CM_CAN_ID_D_T CanID_D;
	CM_CAN_ID_U_T CanID_U;
} CM_CAN_ID_T;

//extern CanTxMsgTypeDef TxMessage;
//extern CanRxMsgTypeDef RxMessage;
extern uint8_t CanUpdataBuff[64];
extern uint8_t CanRxdataBuff[64];

void RxMsgHandle(uint32_t ID,uint8_t* pdata);

void CM_CAN_Init(void);
void RxMsgHandle(uint32_t ID,uint8_t* pdata);
//void CAN_SetMsg(uint32_t ID,uint8_t IDE, uint8_t RTR,uint8_t DLC,uint8_t* Data);
void CAN_SetMsg(void);
//uint32_t TxIdBuild(uint32_t type,uint32_t SrcId, uint32_t DestId,uint32_t ack,uint32_t EndFlag,uint32_t SegmentNum,uint32_t FuncId);
void CM_CanSetMsg(uint32_t id,uint8_t ide,uint8_t rtr,uint8_t dlc,uint8_t* pdata);
void CM_CAN_Tx( mico_can_t can_type, CM_CAN_ID_T id, uint8_t* pdata, uint16_t len );
//CM_CAN_ID_T RxIdPrase(uint32_t id);
CM_CAN_ID_T RxIdPrase(uint32_t id);
//uint32_t TxIdBuild(uint32_t type,uint32_t SrcId, uint32_t DestId,uint32_t ack,uint32_t EndFlag,uint32_t SegmentNum,uint32_t FuncId,uint32_t ResendFlag);
//uint32_t TxIdBuild_U(uint32_t type,uint32_t SrcId, uint32_t DestId,uint32_t ack,uint32_t ResendFlag,uint32_t SegmentNum);
//uint32_t TxIdBuild_D(uint32_t type,uint32_t SrcId, uint32_t DestId,uint32_t Property, uint32_t FuncId,uint32_t SegmentNum,uint32_t ack,uint32_t EndFlag,uint32_t ResendFlag);

uint32_t TxIdBuild(CM_CAN_ID_T* CanId);
void can_protocol_period( void );

/*******************  Bit definition for ExtId bytes  ********************/
#define EXTID_FRAME_TYPE_BITS						((uint32_t)0x10000000)
#define EXTID_SRC_ID_BITS						((uint32_t)0x0FC00000)
#define EXTID_DST_ID_BITS						((uint32_t)0x003F0000)
/* normal */
#define EXTID_PROPERTY_BITS						((uint32_t)0x0000C000)
#define EXTID_FUNC_ID_BITS						((uint32_t)0x00003F80)
#define EXTID_SEGMENT_NUM_D_BITS					((uint32_t)0x00000078)
#define EXTID_ACK_D_BITS						((uint32_t)0x00000004)
#define EXTID_END_FLAG_D_BITS						((uint32_t)0x00000002)
#define EXTID_RESEND_D_BITS						((uint32_t)0x00000001)
/* update */
#define EXTID_ACK_U_BITS						((uint32_t)0x00008000)
#define EXTID_RESEND_U_BITS						((uint32_t)0x00004000)
#define EXTID_SEGMENT_NUM_U_BITS					((uint32_t)0x00003FFF)

#define EXTID_SRC_ID_BITS_NUM						(22U)
#define EXTID_DST_ID_BITS_NUM						(16U)
#define EXTID_PROPERTY_BITS_NUM						(14U)
#define EXTID_FUNC_ID_BITS_NUM						(4U)

#define EXTID_FRAME_TYPE_DATA						(0)
#define EXTID_FRAME_TYPE_UPDATE						(1)
#endif
