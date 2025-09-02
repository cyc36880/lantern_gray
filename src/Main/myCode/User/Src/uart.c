#include "uart.h"

static void HAL_UART_Init(UART_HandleTypeDef* uartHandle);
static void UART_MspInit(UART_HandleTypeDef* uartHandle);

UART_HandleTypeDef huart2;

void MAX_UART2_Init(void)
{
    huart2.Instance = CW_UART2;
    huart2.Init.UART_BaudRate = 9600;
    huart2.Init.UART_Over = UART_Over_16;
    huart2.Init.UART_Source = UART_Source_PCLK;
    huart2.Init.UART_UclkFreq = 48000000;
    huart2.Init.UART_StartBit = UART_StartBit_FE;
    huart2.Init.UART_StopBits = UART_StopBits_1;
    huart2.Init.UART_Parity = UART_Parity_No ;
    huart2.Init.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    huart2.Init.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
    HAL_UART_Init(&huart2);
    
    //使能UARTx RC中断
    UART_ITConfig(CW_UART2, UART_IT_RC, ENABLE);
    UART_ClearITPendingBit(CW_UART2, UART_IT_RC);
}




static void HAL_UART_Init(UART_HandleTypeDef* uartHandle)
{
    UART_MspInit(uartHandle);
    UART_Init(uartHandle->Instance, &uartHandle->Init);
}


static void UART_MspInit(UART_HandleTypeDef* uartHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (uartHandle == &huart2)
    {
//        SYSCTRL_AHBPeriphClk_Enable(SYSCTRL_AHB_PERIPH_GPIOA, ENABLE);
        SYSCTRL_AHBPeriphClk_Enable(SYSCTRL_AHB_PERIPH_GPIOB, ENABLE);
        SYSCTRL_APBPeriphClk_Enable1(SYSCTRL_APB1_PERIPH_UART2, ENABLE);

        GPIO_WritePin(CW_GPIOB, GPIO_PIN_5, GPIO_Pin_SET);    // 设置TXD的默认电平为高，空闲

        GPIO_InitStruct.Pins = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pins = GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);

        //UART TX RX 复用
        PB06_AFx_UART2RXD();
        PB05_AFx_UART2TXD();
    }
}

void uart_send_bytes(UART_HandleTypeDef * uart, uint8_t * buffer, uint8_t len)
{
    while (len--)
    {
        UART_SendData_8bit(uart->Instance, * buffer);
        buffer++;
        while (UART_GetFlagStatus(uart->Instance, UART_FLAG_TXE) == RESET);
    }
}

int uart_receive_bytes(UART_HandleTypeDef * uart, uint8_t * buffer, uint8_t len, uint32_t timeout)
{
    uint32_t tick = GetTick();
    uart->buffer = buffer;
    uart->buffer_len = len;
    uart->receive_count = 0;
    uart->receive_finsh_flag = 0;
    uart->start_receive_flag = 1;
    while (0 == uart->receive_finsh_flag)
    {
        if ( (GetTick() - tick) > timeout)
        {
            return -1;
        }
    }
    return 0;
}

uint32_t count=0;

void UART2_ReceiveCallBack(void)
{
    if (UART_GetITStatus(CW_UART2, UART_IT_RC) != RESET)
    {
        if (1 == huart2.start_receive_flag)
        {
            huart2.buffer[huart2.receive_count] = UART_ReceiveData_8bit(CW_UART2);
            if (++huart2.receive_count >= huart2.buffer_len)
            {
                huart2.receive_finsh_flag = 1;
                huart2.start_receive_flag = 0;
            }
        }
        UART_ClearITPendingBit(CW_UART2, UART_IT_RC);
    }
}

/********************************************************/
/* ****************** prinft ************************** */
/********************************************************/

#ifdef __GNUC__
    /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
    set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
    UART_SendData_8bit(CW_UART2, (uint8_t)ch);

    while (UART_GetFlagStatus(CW_UART2, UART_FLAG_TXE) == RESET);

    return ch;
}

size_t __write(int handle, const unsigned char * buffer, size_t size)
{
    size_t nChars = 0;

    if (buffer == 0)
    {
        /*
         * This means that we should flush internal buffers.  Since we
         * don't we just return.  (Remember, "handle" == -1 means that all
         * handles should be flushed.)
         */
        return 0;
    }


    for (/* Empty */; size != 0; --size)
    {
        UART_SendData_8bit(CW_UART2, *buffer++);
        while (UART_GetFlagStatus(CW_UART2, UART_FLAG_TXE) == RESET);
        ++nChars;
    }

    return nChars;
}
