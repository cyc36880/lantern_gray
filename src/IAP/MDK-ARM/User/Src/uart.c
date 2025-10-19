#include "uart.h"

static void HAL_UART_Init(UART_HandleTypeDef *uartHandle);
static void UART_MspInit(UART_HandleTypeDef *uartHandle);

UART_HandleTypeDef huart1;

void MAX_UART1_Init(void)
{
    huart1.Instance = CW_UART1;
    huart1.Init.UART_BaudRate = 115200;
    huart1.Init.UART_Over = UART_Over_16;
    huart1.Init.UART_Source = UART_Source_PCLK;
    huart1.Init.UART_UclkFreq = 48000000;
    huart1.Init.UART_StartBit = UART_StartBit_FE;
    huart1.Init.UART_StopBits = UART_StopBits_1;
    huart1.Init.UART_Parity = UART_Parity_No;
    huart1.Init.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    huart1.Init.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
    HAL_UART_Init(&huart1);
}

static void HAL_UART_Init(UART_HandleTypeDef *uartHandle)
{
    UART_MspInit(uartHandle);
    UART_Init(uartHandle->Instance, &uartHandle->Init);
}

static void UART_MspInit(UART_HandleTypeDef *uartHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (uartHandle == &huart1)
    {
        SYSCTRL_AHBPeriphClk_Enable(SYSCTRL_AHB_PERIPH_GPIOA, ENABLE);
        SYSCTRL_APBPeriphClk_Enable1(SYSCTRL_APB1_PERIPH_UART1, ENABLE);

        GPIO_WritePin(CW_GPIOB, GPIO_PIN_6, GPIO_Pin_SET); // 设置TXD的默认电平为高，空闲

        GPIO_InitStruct.Pins = GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_Init(CW_GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pins = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
        GPIO_Init(CW_GPIOA, &GPIO_InitStruct);

        // UART TX RX 复用
        PA05_AFx_UART1RXD();
        PA06_AFx_UART1TXD();
    }
}

void uart_send_bytes(uint8_t *data, uint16_t len)
{
    while (len--)
    {
        UART_SendData_8bit(CW_UART1, *data++);
        while (UART_GetFlagStatus(CW_UART1, UART_FLAG_TXE) == RESET)
            ;
    }
}

int uart_receive_bytes(uint8_t *data, uint16_t len, uint32_t timeout)
{
    uint16_t rev_len = 0;
    uint32_t last_tick = GetTick();
    while (GetTick() - last_tick < timeout)
    {
        if (UART_GetFlagStatus(CW_UART1, UART_FLAG_RC) == SET)
        {
            UART_ClearFlag(CW_UART1, UART_FLAG_RC);
            uint8_t rev_data = UART_ReceiveData(CW_UART1);
            data[rev_len++] = rev_data;
        }
        if (rev_len == len)
        {
            return rev_len;
        }
    }
    return rev_len;
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
    UART_SendData_8bit(CW_UART1, (uint8_t)ch);

    while (UART_GetFlagStatus(CW_UART1, UART_FLAG_TXE) == RESET)
        ;

    return ch;
}

size_t __write(int handle, const unsigned char *buffer, size_t size)
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
        UART_SendData_8bit(CW_UART1, *buffer++);
        while (UART_GetFlagStatus(CW_UART1, UART_FLAG_TXE) == RESET)
            ;
        ++nChars;
    }

    return nChars;
}
