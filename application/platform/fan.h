
#ifndef _FAN__H
#define _FAN__H

#include "platform.h"
#include "mico_platform.h"


typedef struct
{
    uint8_t percent;
    uint32_t frq;
}fan_ctrl_t;

void fan_init(void);
#endif
