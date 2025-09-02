#include "adc.h"

static void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle);

ADC_HandleTypeDef adc;




void MAX_ADC_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};

    ADC_InitStructure.ADC_ClkDiv = ADC_Clk_Div1;
    ADC_InitStructure.ADC_ConvertMode = ADC_ConvertMode_Once;    // 单次采样
    ADC_InitStructure.ADC_SQREns = ADC_SqrEns0to0;     // 单通道
    ADC_InitStructure.ADC_IN0.ADC_InputChannel = ADC_InputCH2;
    ADC_InitStructure.ADC_IN0.ADC_SampTime = ADC_SampTime9Clk;
    
    ADC_Init(&ADC_InitStructure);
    ADC_ClearITPendingAll();
    //ADC使能
    ADC_Enable();
    ADC_SoftwareStartConvCmd(ENABLE);


    HAL_ADC_MspInit(&adc);
}



static void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
    PA02_ANALOG_ENABLE();  
}





uint16_t get_adc_val(void)
{
    ADC_SoftwareStartConvCmd(ENABLE); //启动下一次ADC转换
    return  ADC_GetConversionValue(0);
}



