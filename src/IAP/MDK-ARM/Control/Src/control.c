#include "control.h"
#include "light.h"


void setup(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

    __SYSCTRL_GPIOB_CLK_ENABLE();    //Open GPIOA Clk
    __SYSCTRL_GPIOA_CLK_ENABLE();
		
    GPIO_InitStruct.Pins = GPIO_PIN_0|GPIO_PIN_1;  //推挽B0\B1
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init( CW_GPIOB, &GPIO_InitStruct);
    GPIO_WritePin( CW_GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_Pin_RESET);  //高电平
    
		GPIO_InitStruct.Pins = GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2;  //推挽A3\A4
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init( CW_GPIOA, &GPIO_InitStruct);
    GPIO_WritePin( CW_GPIOA, GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2, GPIO_Pin_RESET);  //高电平
	
		
		GPIO_SWD2GPIO();
		
		GPIO_InitStruct.Pins = GPIO_PIN_7;  
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init( CW_GPIOA, &GPIO_InitStruct);
    GPIO_WritePin( CW_GPIOA, GPIO_PIN_7, GPIO_Pin_SET);  //高电平
    
		GPIO_InitStruct.Pins =  GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_InitStruct.IT   = GPIO_IT_NONE;
    GPIO_Init( CW_GPIOA, &GPIO_InitStruct);
}
void loop(void)
{   
   
}






