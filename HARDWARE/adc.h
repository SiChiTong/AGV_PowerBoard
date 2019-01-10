#ifndef __HARDWARE_ADC_H__
#define __HARDWARE_ADC_H__
#include "stm32f10x.h"

void adc_init(void);
uint16_t get_adc(ADC_TypeDef* ADCx, uint8_t ch);
#endif
