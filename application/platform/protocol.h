/* 
*  Author: Adam Huang
*  Date:2016/12/13
*/
#ifndef __PROTOCOL_H
#define __PROTOCOL_H
#include <stdint.h>
#include "serial_uart.h"
#include "RingBufferUtils.h"
#include "voltage_detect.h"
#include "serial_leds.h"


#define PROTOCOL_DEBUG

#define UART_RX_BUFFER_LENGTH            255
#define UART_TX_BUFFER_LENGTH            90

#define FRAME_HEADER                    0x5A
#define FRAME_FOOTER                    0xA5

#define FRAME_TYPE_LEDS_CONTROL         0x01
#define FRAME_TYPE_SYS_STATUS           0x02
#define FRAME_TYPE_BAT_STATUS           0x03
#define FRAME_TYPE_MODULE_STATE         0x04
#define FRAME_TYPE_READ_ERR_CURRENT     0x05//
#define FRAME_TYPE_MODULE_CONTROL       0x06
#define FRAME_TYPE_IRLED_CONTROL        0x07

#define FRAME_TYPE_S_SYS_V_BAT          0x02

#define FRAME_TYPE_FAULT_BIT            0x04


#define FRAME_TYPE_FANS_CONTROL         0x07

#define FRAME_TYPE_TEST_CURRENT         0x0A

#define FRAME_TYPE_VERSION_INFO         0x0E
#define FRAME_TYPE_FW_UPGRADE           0x0F

#define CONTROL_SYSTEM_SHUTDOWN         0x01

#define READ_FW_VERSION_COMMAND         0x00
#define READ_PROTOCOL_VERSION           0x01

#define HW_VERSION                      "01"
#define SW_VERSION                      "NOAHC001M08B102"
#define PROTOCOL_VERSION                "20170619P0001"

typedef struct _serial_frame_t {
  uint8_t               header;
  uint8_t               length;
} serial_frame_t;

typedef struct {
  uint8_t           detectHeader;
  uint8_t           detectLength;
  uint8_t           detectType;
} bufferHeader_t;

typedef struct recBuf_t {
  uint8_t               showLogFlag;
#define         CLOSE_SHOW_LOG  0x00
#define         OPEN_SHOW_LOG   0x01
  uint8_t               pData;
  uint8_t               *rxBuffer;
  bufferHeader_t        bufferHeader;
  uint8_t               rxCount;
  uint32_t              startTime;
#define         COMMUNICATION_TIMEOUT_TIME_MS           6000
  
} recBuf_t;

typedef struct _ram_buff_t {
  const uint8_t         *pBuffer;
  const uint8_t         *offset;
  uint8_t               *pData;
  const uint32_t        bufferSize;
  uint32_t              receiveStartTime;
} ram_buff_t;

typedef struct _serial_t {
  volatile uint8_t              isSerialInitialized;
  volatile uint8_t              isStartDmaReceive;
  recBuf_t                      *rx_info;
  uart_serial_t                 *uart_serial;
  ram_buff_t                    rx_buf;
  ram_buff_t                    tx_buf;
} serial_t;

typedef struct _recSerialLedsFrame_t 
{
  uint8_t               lightMode;
  uint8_t               lightEffectH;
  uint8_t               lightEffectL;
} recSerialLedsFrame_t;

#pragma pack(1)
typedef struct 
{
//  uint8_t               ctype;
  uint8_t               cur_light_mode;
  color_t               color;
  uint8_t               period;
} rcv_serial_leds_frame_t;

typedef struct 
{
  uint8_t               ctype;
  uint8_t               cur_light_mode;
  color_t               color;
  uint8_t               period;
} ack_serial_leds_frame_t;
#pragma pack()

typedef struct _ackSysStatusVbatFrame_t {
  uint8_t               ctype;
  uint8_t               cmd_num;
#define             SYS_VBAT_READ_CMD          0x00
#define             SYS_VCHARGE_READ_CMD       0x01
  uint16_t              sysStatus;
  uint16_t              voltage;
} ackSysStatusVbatFrame_t;

#pragma pack(1)
typedef struct
{
    //uint8_t             ctype;
    uint8_t             cmd_num;
}rcv_sys_status_t;
typedef struct
{
    uint8_t             ctype;
    uint8_t             cmd_num;
    uint16_t            sys_status;    
}ack_sys_status_t;

typedef struct
{
    uint8_t             ctype;
    uint8_t             cmd_num;
    uint16_t            bat_status;
}ack_bat_status_t;
#pragma pack()



typedef struct _ackModuleStatusFrame_t {
  uint8_t               ctype;
  uint8_t               cmd_num;
#define             MODULE_STATUS_CMD_NUM             0x01
  uint8_t               module_status[4];
} ack_module_status_frame_t;

typedef struct _ackFaultStatusFrame_t {
  uint8_t               ctype;
  uint8_t               cmd_num;
#define             FAULT_STATUS_CMD_NUM              0x01
  uint8_t               faultBit[4];
} ackFaultStatusFrame_t;



#if 1// do not delete !
#define             SYSTEM_MODULE         0x00
#define             MOTOR_MODULE          0x01
#define             SENSOR_MODULE         0x02
#define             LEDS_MODULE           0x03
#define             _5VRESERVE_MODULE     0x04
#define             PAD_MODULE            0x05
#define             _12V_ROUTER_MODULE    0x06
#define             _2_1_PA_MODULE        0x07
#define             DYP_MODULE            0x08
#define             X86_MODULE            0x09
#define             NV_MODULE             0x0A
#define             DLP_MODULE            0x0B
#define             _12V_RESERVE_MODULE   0x0C
#define             PRINTER_MODULE        0x0D
#define             _24V_RESERVE_MODULE   0x0E
#define             BAT_NV_MODULE         0x0F
#define             _5V_ALL_MODULE        0x10
#define             _12V_ALL_MODULE       0x11
#define             _24V_ALL_MODULE       0x12
#define             AIUI_MODULE           0x13
#define             _5V_ROUTER_MODULE     0x14
#endif

typedef struct _recModuleControlFrame_t {
  //uint8_t               module;
  uint32_t               module;

  uint8_t               control;
} rcv_module_control_frame_t;

typedef struct _recIRLedControlFrame_t {
  uint8_t               type_wr;
  uint8_t               brightness;
} rcv_irled_control_frame_t;

#pragma pack(1)
typedef struct _ackGeneralFrame_t {
  uint8_t               ctype;
  uint32_t               ack;
#define             ACK_SUCCESS           0x00
#define             ACK_FAIL              0x01
#define             HW_NO_SUPPORT         0xFFFFFFFF
} ackGeneralFrame_t;
#pragma pack()

typedef struct _ackVersionInfoFrame_t {
  uint8_t               ctype;
  uint8_t               ver_type;
#define             VERSION_TYPE_FARMWARE 0x00
#define             VERSION_TYPE_PROTOCOL 0x01
  uint8_t               hw[3];
  uint8_t               sw[16];           
} ackVersionInfoFrame_t;

/***************** begin of upgrade defines ********************/

#define UPGRADE_FRAME_TYPE_PREPARE      0x00
#define UPGRADE_FRAME_TYPE_GOING        0x01
#define UPGRADE_FRAME_TYPE_FINISH       0x02

#define MCU_PREPARE_OK                  0x00
#define MCU_STORAGE_NOT_ENOUGH          0x01
#define MCU_RETRY_LATER                 0x02

#define PACKGE_PROCESS_OK               0x00
#define PACKGE_PROCESS_FAIL             0x01

#define FIRMWARE_CHECKSUM_RIGHT         0x00
#define FIRMWARE_CHECKSUM_ERR           0x01

typedef struct _recPrepareUpgradeFrame_t {
  uint8_t               upgradeFrameType;
  uint8_t               md5[16];
  uint8_t               firmwareSize[4];
} recPrepareUpgradeFrame_t;

typedef struct _recFirmwarePackageFrame_t {
  uint8_t               upgradeFrameType;
  uint8_t               *pPackageData;
  uint32_t              packageDataLength;
} recFirmwarePackageFrame_t;

typedef struct _recUpgradeFinishCheckFrame_t {
  uint8_t               upgradeFrameType;
  uint8_t               finishFlag;
} recUpgradeFinishCheckFrame_t;

typedef struct _upgradeGeneralAckFrame_t {
  uint8_t               ctype;
  uint8_t               upgradeFrameType;
  uint8_t               ackState;
} upgradeGeneralAckFrame_t;

typedef struct _recTestCurrentCmdFrame_t {
  uint8_t               cmdType;
  uint8_t               command;
  uint8_t               sendRate;
#define                         SEND_RATE_SINGLE        ((uint8_t)0x00)
#define                         SEND_RATE_1HZ           ((uint8_t)0x01)
#define                         SEND_RATE_2HZ           ((uint8_t)0x02)
#define                         SEND_RATE_5HZ           ((uint8_t)0x03)
#define                         SEND_RATE_10HZ          ((uint8_t)0x04)
#define                         SEND_RATE_50HZ          ((uint8_t)0x05)
#define                         SEND_RATE_100HZ         ((uint8_t)0x06)
#define                         SEND_RATE_0_5HZ         ((uint8_t)0x07)
#define                         SEND_RATE_0_2HZ         ((uint8_t)0x08)
#define                         SEND_RATE_0_1HZ         ((uint8_t)0x09) 
} recTestCurrentCmdFrame_t;

#define         IS_SEND_RATE_DATA(x) (((x) == SEND_RATE_SINGLE) ||\
                                      ((x) == SEND_RATE_1HZ) ||\
                                      ((x) == SEND_RATE_2HZ) ||\
                                      ((x) == SEND_RATE_5HZ) || \
                                      ((x) == SEND_RATE_10HZ) ||\
                                      ((x) == SEND_RATE_50HZ) ||\
                                      ((x) == SEND_RATE_100HZ) ||\
                                      ((x) == SEND_RATE_0_5HZ) ||\
                                      ((x) == SEND_RATE_0_2HZ) ||\
                                      ((x) == SEND_RATE_0_1HZ) )

typedef struct _uploadCurrentInfoFrame_t {
  uint8_t               ctype;
  uint8_t               cmdType;
  voltageData_t         currentInfo;  
  uint8_t               faultBit[4];
  uint8_t               sendRate;
  uint8_t               reserve[7];
} uploadCurrentInfoFrame_t;

typedef struct _errInfoFrame_t {
  uint8_t               ctype;
  uint8_t               errChannel;
  uint16_t              errData;  
} errInfoFrame_t;


typedef struct
{
    uint8_t start_flag;
    uint8_t end_flag;
    uint8_t data_len;
    uint8_t rcv_cnt;
    uint8_t rcv_buf[50];
}led_com_opt_t;

/***************** end of upgrade defines *********************/
extern serial_t * const serial;

void protocol_period( void );
OSStatus Protocol_Init( void );
OSStatus uploadCurrentInformation( serial_t *serial, voltageData_t *voltageInfo );
uint32_t sendRateToTime(uint8_t sendRate);
//OSStatus Protocol_Init( serial_t *serial );

#endif
