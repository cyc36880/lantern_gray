#ifndef __uart_h__
#define __uart_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef struct 
{
    UART_TypeDef* Instance;
    UART_InitTypeDef Init;
    uint8_t * buffer;
    uint16_t buffer_len;
    uint16_t receive_count;
    uint8_t start_receive_flag : 1;
    uint8_t receive_finsh_flag : 1; 
} UART_HandleTypeDef;


void MAX_UART2_Init(void);
void uart_send_bytes(UART_HandleTypeDef * uart, uint8_t * buffer, uint8_t len);
int uart_receive_bytes(UART_HandleTypeDef * uart, uint8_t * buffer, uint8_t len, uint32_t timeout);

extern UART_HandleTypeDef huart2;

#ifdef __cplusplus
}
#endif

#endif /* __uart_h__ */
