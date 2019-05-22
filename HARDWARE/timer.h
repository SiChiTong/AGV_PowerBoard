#ifndef __HARDWARE_TIMER_H__
#define __HARDWARE_TIMER_H__
#include "stm32f10x.h"

void timer_1_ch1_pwm_init(u16 arr,u16 psc, uint16_t pulse);
void tim2_ch1_pwm_init(u16 arr,u16 psc, uint16_t pulse);
void tim2_ch2_pwm_init(u16 arr,u16 psc, uint16_t pulse);
void tim3_ch1_pwm_init(u16 arr,u16 psc, uint16_t pulse);

void change_tim2_ch1_pwm_duty(uint16_t pulse);
void change_tim2_ch2_pwm_duty(uint16_t pulse);
void change_tim3_ch1_pwm_duty(uint16_t pulse);
#endif

