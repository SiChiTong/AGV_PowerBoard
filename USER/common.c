/*
 *  Author: Kaka Xie
 *  brief: user configuration ucosii
 */

#include "common.h"
#include "power_on_off_task.h"

extern void falsh_test_task(void *pdata);

static void task_create(void)
{
//    OSTaskCreate(fp_uart_com_send_task,     (void *)0,  (OS_STK*)&FP_UART_COM_SEND_TASK_STK[FP_UART_COM_SEND_TASK_STK_SIZE - 1],            FP_UART_COM_SEND_TASK_PRIO);
//    OSTaskCreate(fp_uart_com_rcv_task,      (void *)0,  (OS_STK*)&FP_UART_COM_RCV_TASK_STK[FP_UART_COM_RCV_TASK_STK_SIZE - 1],              FP_UART_COM_RCV_TASK_PRIO);

    OSTaskCreate(indicator_led_task,        (void *)0,  (OS_STK*)&INDICATOR_LED_TASK_STK[INDICATOR_LED_STK_SIZE-1],                         INDICATOR_LED_TASK_PRIO);

//    OSTaskCreate(display_task,              (void *)0,  (OS_STK*)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE - 1],                                   DISPLAY_TASK_PRIO);
//    OSTaskCreate(lc12s_send_task,           (void *)0,  (OS_STK*)&LC12S_UART_COM_SEND_TASK_STK[LC12S_UART_COM_SEND_TASK_STK_SIZE - 1],      LC12S_UART_COM_SEND_TASK_PRIO);
//    OSTaskCreate(lc12s_rcv_task,            (void *)0,  (OS_STK*)&LC12S_UART_COM_RCV_TASK_STK[LC12S_UART_COM_RCV_TASK_STK_SIZE - 1],        LC12S_UART_COM_RCV_TASK_PRIO);




//    OSTaskCreate(unlock_task,               (void *)0,  (OS_STK*)&UNLOCK_TASK_STK[UNLOCK_TASK_STK_SIZE - 1],                                UNLOCK_TASK_PRIO);
//    OSTaskCreate(lock_status_task,          (void *)0,  (OS_STK*)&LOCK_STATUS_TASK_STK[LOCK_STATUS_TASK_STK_SIZE - 1],                      LOCK_STATUS_TASK_PRIO);

    OSTaskCreate(battery_task,              (void *)0,  (OS_STK*)&BATTERY_TASK_STK[BATTERY_TASK_STK_SIZE - 1],                              BATTERY_TASK_PRIO);
    OSTaskCreate(can_protocol_task,         (void *)0,  (OS_STK*)&can_protocol_task_stk[CAN_PROTOCOL_TASK_STK_SIZE - 1],                    CAN_RPOTOCOL_TASK_PRIO);
    OSTaskCreate(power_on_task,             (void *)0,  (OS_STK*)&power_on_stk[POWER_ON_STK_SIZE - 1],                                      POWER_ON_TASK_PRIO);

}

static void sem_create(void)
{
//    fp_uart_data_come_sem = OSSemCreate(5);
//    fp_com_get_feature_sem = OSSemCreate(0);
//    fp_com_read_feature_sem = OSSemCreate(0);
//    fp_com_set_feature_sem = OSSemCreate(0);

//    wireless_com_data_come_sem = OSSemCreate(0);

//    unlock_sem = OSSemCreate(0);
}


static int mem_create(void)
{
//    uint8_t err = 0;
//    fp_rcv_mem_handle = OSMemCreate((void *)&fp_rcv_mem[0][0], sizeof(fp_rcv_mem) / sizeof(fp_rcv_mem[0]), sizeof(fp_rcv_buf_t), &err);
//    if(fp_rcv_mem_handle == 0)
//    {
//        /*
//        todo: err process
//        */
//        return -1;
//    }


//    fp_short_ack_mem_handle = OSMemCreate((void *)&fp_short_ack_mem[0][0], sizeof(fp_short_ack_mem) / sizeof(fp_short_ack_mem[0]), sizeof(fp_short_ack_t), &err);
//    if(fp_short_ack_mem_handle == NULL)
//    {
//        /*
//        todo: err process
//        */
//        return -1;
//    }



//    fp_long_ack_mem_handle = OSMemCreate((void *)&fp_long_ack_mem[0][0], sizeof(fp_long_ack_mem) / sizeof(fp_long_ack_mem[0]), sizeof(fp_long_ack_t), &err);
//    if(fp_long_ack_mem_handle == NULL)
//    {
//        /*
//        todo: err process
//        */
//        return -1;
//    }


    return 0;
}

static int queue_create(void)
{
//    fp_short_ack_queue_handle = OSQCreate(&fp_short_ack_queue_p[0], FP_SHORT_ACK_QUEUE_NUM);
//    if(fp_short_ack_queue_handle == NULL)
//    {
//        /*
//        todo: err process
//        */
//        return -1;
//    }


//    fp_long_ack_queue_handle = OSQCreate(&fp_long_ack_queue_p[0], FP_LONG_ACK_QUEUE_NUM);
//    if(fp_long_ack_queue_handle == NULL)
//    {
//        /*
//        todo: err process
//        */
//        return -1;
//    }
    return 0;
}

static void os_user_config(void)
{
    sem_create();
    mem_create();
    queue_create();
    task_create();
}

static void user_init_depend_on_os_config(void)
{
//    fp_rcv_buf_head_init();
}

void user_init(void)
{
    os_user_config();
    user_init_depend_on_os_config();
}

