#include "common.h"



// 串口收一个字节，带超时（单位 ms），返回 0 成功，-1 失败
int uart_recv_byte(uint8_t *byte, uint32_t timeout_ms)
{
    return uart_receive_bytes(byte, 1, timeout_ms);
}

// 串口发送一个字节
void uart_send_byte(uint8_t byte)
{
    uart_send_bytes(&byte, 1);
}

// 擦除 Flash 起始地址开始的区域，len 单位字节
int flash_erase(uint32_t addr, uint32_t len)
{
    return 0;
}

// 写 Flash 地址 addr 处，长度 len 字节，data 来自接收到的 XMODEM 数据包
int flash_write(uint32_t addr, const uint8_t *data, uint32_t len)
{
    return 0;
}




