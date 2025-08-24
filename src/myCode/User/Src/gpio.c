#include "gpio.h"



void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __SYSCTRL_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.IT = GPIO_IT_NONE;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pins = WS2812_GPIO_Pin;
    GPIO_Init(WS2812_GPIO_Port, &GPIO_InitStruct);
    
    GPIO_WritePin(WS2812_GPIO_Port, WS2812_GPIO_Pin, GPIO_Pin_RESET);
}

