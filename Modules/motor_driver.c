/*
 *  Author: Kaka Xie
 *  brief: motor driver
 */

#include "motor_driver.h"

void motor_driver_init(u16 arr, u16 psc, uint16_t pulse)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = arr;//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;//设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;//设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    TIM_ARRPreloadConfig(TIM1, ENABLE);//使能ARR预装载，防止向上计数时更新事件异常延迟

    /*************设置占空比，将TIM1_CH1配置成pwm输出***************/
    //初始化TIM1_CH1 PWM输出
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性:TIM输出比较极性低

    TIM_OCInitStructure.TIM_Pulse = pulse;
    TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);//将配置参数初始化外设TIM1 OC1

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);//使能TIM1在CCR1上的预装载寄存器

    TIM_Cmd(TIM1, ENABLE);//使能TIM1
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

}

void set_motor_speed(uint16_t speed)
{
    motor_driver_init(720, 1, 720 * (100 - speed) / 100);
}

void set_motor_forward(uint16_t speed)
{
    /*
    TODO: set forward
    */
    motor_driver_init(720, 1, 720 * (100 - speed) / 100);
}

void set_motor_backward(uint16_t speed)
{
    /*
    TODO: set backward
    */
    motor_driver_init(720, 1, 720 * (100 - speed) / 100);
}

