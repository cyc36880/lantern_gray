#include "./myMain.h"

#include "../Drive/inc/ws2812.h"
#include "register.h"
#include "uart.h"

static const uint8_t filter_depth = 3;
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
    static uint8_t uart_rev[filter_depth][3] = {0};
    static uint8_t sampling_count = 0;
    
    if (1 == sys_register.changle_flag)
    {
        sys_register.changle_flag = 0;
        uint8_t r = (uint32_t)sys_register.reg[2 + 0] * sys_register.reg[1] / 255;
        uint8_t g = (uint32_t)sys_register.reg[2 + 1] * sys_register.reg[1] / 255;
        uint8_t b = (uint32_t)sys_register.reg[2 + 2] * sys_register.reg[1] / 255;
        for (uint16_t i=0; i<WS2812_NUM; i++)
        {
            ws2812_set_color(i, r, g, b);
        }
        ws2812_sync_display();
    }
    uart_send_bytes(&huart2, (uint8_t *)&ultr_start, 1);
    
    
    if (uart_receive_bytes(&huart2, uart_rev[sampling_count], 3, 200) == 0)
    {
        if (++sampling_count >= filter_depth) sampling_count=0;
        
    }
    if (0 == sampling_count)
    {
        // 对uart_rev[x]冒泡排序
        for (uint8_t i=0; i<filter_depth; i++)
        {
            uint32_t temp_val = uart_rev[i][0] << 16 | uart_rev[i][1] << 8 | uart_rev[i][2];
            for (uint8_t j=i+1; j<filter_depth; j++)
            {
                uint32_t next_val = uart_rev[j][0] << 16 | uart_rev[j][1] << 8 | uart_rev[j][2];
                if (next_val < temp_val)
                {
                    // 交换
                    uint8_t tmp[3] = {uart_rev[i][0], uart_rev[i][1], uart_rev[i][2]};
                    uart_rev[i][0] = uart_rev[j][0];
                    uart_rev[i][1] = uart_rev[j][1];
                    uart_rev[i][2] = uart_rev[j][2];
                    uart_rev[j][0] = tmp[0];
                    uart_rev[j][1] = tmp[1];
                    uart_rev[j][2] = tmp[2];
                    temp_val = next_val;
                }
            }
        }

        ultr_register.reg[0] = uart_rev[filter_depth/2][0];
        ultr_register.reg[1] = uart_rev[filter_depth/2][1];
        ultr_register.reg[2] = uart_rev[filter_depth/2][2];
    }
}

