#ifndef __USER_OS_COMMON_H__
#define __USER_OS_COMMON_H__
#include "stm32f10x.h"
#include "ucos_ii.h"

#include "led_task.h"
#include "lock_task.h"
#include "battery_task.h"



typedef enum
{
    LC12S_UART_COM_RCV_TASK_PRIO = 4,
    FP_UART_COM_RCV_TASK_PRIO,

    LC12S_UART_COM_SEND_TASK_PRIO,
    DIS_TEST_TASK_PRIO,
    FP_UART_COM_SEND_TASK_PRIO,
    UNLOCK_TASK_PRIO,
    FP_RECORD_TASK_PRIO,
    DISPLAY_TASK_PRIO,
    LOCK_STATUS_TASK_PRIO,
    BATTERY_TASK_PRIO,
    LED1_TASK_PRIO,
    INDICATOR_LED_TASK_PRIO
}task_prio_e;


void os_user_config(void);
void user_init_depend_on_os_config(void);
extern OS_MEM *fp_rcv_mem_handle;


uint32_t get_tick(void);
void user_init(void);
#endif

