/*
 *  Author: Kaka Xie
 *  brief: lock task
 */

#include "lock_task.h"
#include "delay.h"
#include "lock.h"
#include "display_task.h"

OS_STK UNLOCK_TASK_STK[UNLOCK_TASK_STK_SIZE];

OS_STK LOCK_STATUS_TASK_STK[LOCK_STATUS_TASK_STK_SIZE];

OS_EVENT *unlock_sem;

void start_to_unlock(void)
{
    OSSemPost(unlock_sem);
}

void unlock(void)
{
    lock_on();
    delay_ms(100);
    lock_off();
}

volatile uint8_t lock_status = 0;
void unlock_task(void *pdata)
{
    uint8_t err = 0;
    while(1)
    {
        OSSemPend(unlock_sem, 0, &err);
        if(lock_status == LOCK_STATUS_LOCK)
        {
            unlock();
            delay_ms(500);
        }
        else
        {
            /*
            todo: lock is already unlock
            */
        }
    }
}

void lock_status_task(void *pdata)
{
    uint16_t cnt = 0;
    uint8_t pre_lock_status = 0;
    while(1)
    {
        if(pre_lock_status == get_lock_status())
        {
            cnt++;
            if(cnt >= 10)
            {
                lock_status = pre_lock_status;
                cnt = 11;
            }
        }
        else
        {
            cnt = 0;
        }
        pre_lock_status = get_lock_status();
        delay_ms(50);
    }
}
