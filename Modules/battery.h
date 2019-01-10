#ifndef __MODULE__BATTERY_H__
#define __MODULE__BATTERY_H__
#include "stm32f10x.h"

void battery_adc_init(void);
uint16_t get_battery_voltage(void);
#endif
