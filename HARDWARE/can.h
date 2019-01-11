#ifndef HARDWARE_CAN__H
#define HARDWARE_CAN__H


#include "stm32f10x.h"





#define POWERBOARD_CAN_MAC_SRC_ID       0x0052
#define CAN_FILTER_ID                   (POWERBOARD_CAN_MAC_SRC_ID << 13)
#define CAN_FILTER_MASK                 (0x00ff << 13)



//CAN1
#define CAN1_GPIO_RX_PORT		GPIOA
#define CAN1_GPIO_RX_PIN		GPIO_Pin_11
#define CAN1_GPIO_TX_PORT		GPIOA
#define CAN1_GPIO_TX_PIN		GPIO_Pin_12



typedef union
{
	struct
	{
		uint32_t SourceID  : 8;
		uint32_t FUNC_ID   : 4;
		uint32_t ACK       : 1;
		uint32_t DestMACID : 8;
		uint32_t SrcMACID  : 8;
		uint32_t res       : 3;
	}CanID_Struct;
	uint32_t  CANx_ID;
}CAN_ID_UNION;


typedef union
{
	struct
	{
        uint8_t SegNum  : 6;
        uint8_t SegPolo : 2;
		uint8_t Data[7];
	}CanData_Struct;
	uint8_t CanData[8];
}CAN_DATA_UNION;


extern void CanLongBufInit(void);
extern uint8_t CanInit(void);
#endif
