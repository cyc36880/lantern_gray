#include "./myMain.h"

#include "../Drive/inc/ws2812.h"
#include "register.h"
#include "uart.h"

static const uint8_t ultr_start = 0xA0;

void setup(void)
{
    SysTickDelay(20); // 等待上电稳定
    for (uint8_t j=0; j<3; j++)
    {
        for (uint16_t i=0; i<WS2812_NUM; i++)
        {
            ws2812_set_color(i, 0, 0, 0);
        }
        ws2812_sync_display();
    }
}



void loop(void)
{
    if (1 == sys_register.changle_flag)
    {
        sys_register.changle_flag = 0;
        uint8_t r = sys_register.reg[2 + 0] * sys_register.reg[1] / 255;
        uint8_t g = sys_register.reg[2 + 1] * sys_register.reg[1] / 255;
        uint8_t b = sys_register.reg[2 + 2] * sys_register.reg[1] / 255;
        for (uint16_t i=0; i<WS2812_NUM; i++)
        {
            ws2812_set_color(i, r, g, b);
        }
        ws2812_sync_display();
    }
    uart_send_bytes(&huart2, (uint8_t *)&ultr_start, 1);
    uint8_t uart_rev[3];
    if (uart_receive_bytes(&huart2, uart_rev, 3, 200) == 0)
    {
        ultr_register.reg[0] = uart_rev[0];
        ultr_register.reg[1] = uart_rev[1];
        ultr_register.reg[2] = uart_rev[2];
    }
}

