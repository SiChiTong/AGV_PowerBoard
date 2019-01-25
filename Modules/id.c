/*
 *  Author: Kaka Xie
 *  brief: get my id
 */

#include "id.h"
#include "delay.h"
#include "platform.h"



uint8_t get_id_key_value(id_key_enum key)
{
    switch(key)
    {
        case ID_KEY_1:
            return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0);
        case ID_KEY_2:
            return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1);

        default:
            return 0xff;
    }
}


uint8_t get_hardware_version(void)
{

#define DEBOUNCE_TIME       1 * OS_TICKS_PER_SEC / 10
    uint8_t new_key_value = 0;
    uint8_t old_key_value = 0;
    uint32_t cnt = 0;
    uint32_t start_tick = get_tick();

    while(cnt <= 5)
    {
        old_key_value = new_key_value;

        new_key_value |=  get_id_key_value(ID_KEY_1);
        new_key_value |=  get_id_key_value(ID_KEY_2) << 1;

        if(new_key_value != old_key_value)
        {
            cnt = 0;
        }
        delay_us_ex(10 * 1000);
        cnt++;
        if(get_tick() - start_tick >= DEBOUNCE_TIME)
        {
            new_key_value = 0;
            break;
        }
    }

    if((new_key_value != 0) && (new_key_value <= 0x03))
    {
        return new_key_value;
    }

    return 0xff;

}

