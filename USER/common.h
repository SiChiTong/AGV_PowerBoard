#ifndef __USER_OS_COMMON_H__
#define __USER_OS_COMMON_H__
#include "stm32f10x.h"
#include "ucos_ii.h"

#include "led_task.h"
#include "battery_task.h"
#include "can_protocol_task.h"
#include "power_on_off_task.h"
#include "switch_task.h"
#include "charge_task.h"

typedef enum
{
    CAN_RPOTOCOL_TASK_PRIO = 5,
    CHARGE_TASK_PRIO,
//    UNLOCK_TASK_PRIO,
//    LOCK_STATUS_TASK_PRIO,
    SWITCH_TASK_PRIO,
    BATTERY_TASK_PRIO,
    POWER_ON_OFF_TASK_PRIO,
    X86_POWER_ON_OFF_TASK_PRIO,
    RK_POWER_ON_OFF_TASK_PRIO,
    INDICATOR_LED_TASK_PRIO
}task_prio_e;


void os_user_config(void);
void user_init_depend_on_os_config(void);
extern OS_MEM *fp_rcv_mem_handle;


uint32_t get_tick(void);
void user_init(void);
#endif

