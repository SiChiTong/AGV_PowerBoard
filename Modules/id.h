#ifndef __MODULE_ID_H__
#define __MODULE_ID_H__
#include "sys.h"


typedef enum
{
    ID_KEY_1 = 1,
    ID_KEY_2,
    ID_KEY_3,
    ID_KEY_4,
    ID_KEY_5,
    ID_KEY_6
}id_key_enum;

void id_key_init(void);
uint8_t get_hardware_version(void);

#endif
