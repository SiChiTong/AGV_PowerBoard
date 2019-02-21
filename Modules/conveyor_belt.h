#ifndef __CONVEYOR_BELT_H__
#define __CONVEYOR_BELT_H__

#include "stm32f10x.h"
#include "ucos_ii.h"


#define CONVEYOR_BELT_STATUS_STOP       0x00
#define CONVEYOR_BELT_STATUS_LOAD       0x01
#define CONVEYOR_BELT_STATUS_UNLOAD     0x02

#define CONVEYOR_BELT_LOAD_ERROR        0xF0
#define CONVEYOR_BELT_UNLOAD_ERROR      0xF1
#define CONVEYOR_BELT_STATUS_ERROR      0xFF


typedef struct
{
    uint8_t cur_status;
    uint8_t err_status;
    uint8_t pho_switch_status;
    uint8_t motor_speed;
    uint8_t motor_dir;
}conveyor_belt_t;

void conveyor_belt_init(void);
uint8_t set_conveyor_belt_load(void);
uint8_t set_conveyor_belt_unload(void);
uint8_t set_conveyor_belt_stop(void);
#endif
