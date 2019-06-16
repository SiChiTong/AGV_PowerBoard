/*
 *  Author: Kaka Xie
 *  brief: power on self test(POST)
*/


#include "power_on_self_test_task.h"
#include "platform.h"
#include "delay.h"

OS_STK POST_task_stk[POWER_ON_SELF_TEST_STK_SIZE] = {0};
OS_EVENT *start_POST_sem;

void POST_task(void *pdata)
{
    uint8_t err = 0;
    delay_ms(1000);
    for(;;)
    {
        OSSemPend(start_POST_sem, 0, &err);
        delay_ms(2000);
        led_ctrl_wifi_status(LED_STATUS_OK);
        led_ctrl_battery_status(LED_STATUS_OK);
        led_ctrl_trans_status(LED_STATUS_OK);
        delay_ms(2000);
        led_ctrl_wifi_status(LED_STATUS_OFF);
        led_ctrl_battery_status(LED_STATUS_OFF);
        led_ctrl_trans_status(LED_STATUS_OFF);
        delay_ms(500);
        led_ctrl_wifi_status(LED_STATUS_ERR);
        led_ctrl_battery_status(LED_STATUS_ERR);
        led_ctrl_trans_status(LED_STATUS_ERR);
        delay_ms(2000);
        led_ctrl_wifi_status(LED_STATUS_OFF);
        led_ctrl_battery_status(LED_STATUS_OFF);
        led_ctrl_trans_status(LED_STATUS_OFF);
        delay_ms(500);
        led_ctrl_wifi_status(LED_STATUS_WARN);
        led_ctrl_battery_status(LED_STATUS_WARN);
        led_ctrl_trans_status(LED_STATUS_WARN);
        delay_ms(2000);
        led_ctrl_wifi_status(LED_STATUS_OFF);
        led_ctrl_battery_status(LED_STATUS_OFF);
        led_ctrl_trans_status(LED_STATUS_OFF);
        delay_ms(500);
        power_ctrl(POWER_CAMERA_FRONT_LED | POWER_CAMERA_BACK_LED, MODULE_POWER_ON, 1);
        power_ctrl(POWER_HEAD_CAMERA_LED, MODULE_POWER_ON, 2);
        delay_ms(3000);
        power_ctrl(POWER_CAMERA_FRONT_LED | POWER_CAMERA_BACK_LED, MODULE_POWER_OFF, 1);
        power_ctrl(POWER_HEAD_CAMERA_LED, MODULE_POWER_OFF, 2);
        delay_ms(1000);
        OSTaskDel(OS_PRIO_SELF);
    }
}



