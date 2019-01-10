/*
 *  Author: Kaka Xie
 *  brief: get my id
 */

#include "id.h"
#include "delay.h"

void id_key_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);   //使能端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;  //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //input
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //根据设定参数初始化GPIO

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

}

uint8_t get_id_key_value(id_key_enum key)
{
    switch(key)
    {
        case ID_KEY_1:
            return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
        case ID_KEY_2:
            return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
        case ID_KEY_3:
            return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);
        case ID_KEY_4:
            return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6);
        case ID_KEY_5:
            return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
        case ID_KEY_6:
            return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
        default:
            return 0xff;
    }
}


uint8_t get_my_id(void)
{

#define DEBOUNCE_TIME       100/SYSTICK_PERIOD
    uint8_t new_key_value = 0;
    uint8_t old_key_value = 0;
    uint32_t cnt = 0;

    while(cnt <= 5)
    {
        old_key_value = new_key_value;

        new_key_value |=  get_id_key_value(ID_KEY_1);
        new_key_value |=  get_id_key_value(ID_KEY_2) << 1;
        new_key_value |=  get_id_key_value(ID_KEY_3) << 2;
        new_key_value |=  get_id_key_value(ID_KEY_4) << 3;
        new_key_value |=  get_id_key_value(ID_KEY_5) << 4;
        new_key_value |=  get_id_key_value(ID_KEY_6) << 5;

        if(new_key_value != old_key_value)
        {
            cnt = 0;
        }
        delay_us_ex(10 * 1000);
        cnt++;
    }

    if((new_key_value != 0) && (new_key_value <= 0x3f))
    {
        return new_key_value;
    }

    return 0xff;

}

