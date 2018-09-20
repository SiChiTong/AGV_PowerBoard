/* 
*  Author: Adam Huang
*  Date:2017/6/22
*/

#ifndef __BATTERY_H
#define __BATTERY_H

#include "mico.h"

typedef struct _battery_protocol_t {
  uint8_t       soi;
  uint8_t       ver;
  uint8_t       adr;
  uint8_t       cid1;
  uint8_t       cid2;
  uint8_t       length_h;
  uint8_t       length_l;
  uint8_t       *info;
  uint8_t       chksum_h;
  uint8_t       chksum_l;
  uint8_t       eoi;
} battery_protocol_t;


typedef struct _battery_pack_t{
 int16_t pack_current;
 uint32_t pack_voltage;
 uint32_t pack_current_soc;
 uint32_t pack_totoal_soc;
 uint16_t pack_recharge_cycle;
 uint16_t pack_temperature[15];
 uint16_t  percentage;
 uint8_t  com_status;
} battery_pack_t;

extern battery_pack_t battery_pack;

OSStatus battery_init( void );
void battery_protocol_init( void );
void battery_data_recieved( void );
void battery_period( void );
void PrintBatInfo(void);
#endif