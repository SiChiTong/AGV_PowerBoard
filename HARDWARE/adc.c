#include "adc.h"

/**
 * @brief ADC初始化
 * @param none
 * @retval none
 * @note  初始化PA.00为ADC1_CH1，单次转换，软件触发ADC转换
 */
void adc_init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_ADC3 | RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC3通道时钟


    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

    //PF7 作为模拟通道输入引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    ADC_DeInit(ADC3);  //复位ADC3,将外设 ADC3 的全部寄存器重设为缺省值
    ADC_DeInit(ADC1);  //复位ADC3,将外设 ADC3 的全部寄存器重设为缺省值

    //	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 2;	//顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC3, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器
    ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器
    ADC_TempSensorVrefintCmd(ENABLE);

    ADC_Cmd(ADC3, ENABLE);	//使能指定的ADC3
    ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC3

    ADC_ResetCalibration(ADC3);	//使能复位校准
    ADC_ResetCalibration(ADC1);	//使能复位校准

    while(ADC_GetResetCalibrationStatus(ADC3));	//等待复位校准结束

    ADC_StartCalibration(ADC3);	 //开启AD校准

    while(ADC_GetCalibrationStatus(ADC3));	 //等待校准结束


    while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束

    ADC_StartCalibration(ADC1);	 //开启AD校准

    while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
}


/**
 * @brief 获得ADC值
 * @param ADCx: ADC1 / ADC2 / ADC3
 * @param ch:通道值 ADC_Channel_0 ~ ADC_Channel_17
 * @retval ADC转换值
 * @note
 */
uint16_t get_adc(ADC_TypeDef* ADCx, uint8_t ch)
{
    /* ADC的规则通道组设置：一个序列，采样时间239.5周期	*/
    ADC_RegularChannelConfig(ADCx, ch, 1, ADC_SampleTime_239Cycles5 );

    ADC_SoftwareStartConvCmd(ADCx, ENABLE);					//开始转换
    while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC ));	//等待转换结束
    return ADC_GetConversionValue(ADCx);						//返回ADC1转换结果
}
