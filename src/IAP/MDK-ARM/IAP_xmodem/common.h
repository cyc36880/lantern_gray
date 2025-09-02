#ifndef __COMMON_H__
#define __COMMON_H__


#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"
#include "uart.h"

int uart_recv_byte(uint8_t *byte, uint32_t timeout_ms);
void uart_send_byte(uint8_t byte);
int flash_erase(uint32_t addr, uint32_t len);
int flash_write(uint32_t addr, const uint8_t *data, uint32_t len);


#ifdef __cplusplus
}
#endif


#endif /* __COMMON_H__ */

