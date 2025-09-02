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
} UART_HandleTypeDef;


void MAX_UART1_Init(void);
void uart_send_bytes(uint8_t *data, uint16_t len);
int uart_receive_bytes(uint8_t *data, uint16_t len, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* __uart_h__ */
