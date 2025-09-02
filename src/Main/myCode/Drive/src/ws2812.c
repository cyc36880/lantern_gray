#include "../inc/ws2812.h"

#define WS2812_PIN_H_LEVEL()  PA00_SETHIGH() 
#define WS2812_PIN_L_LEVEL()  PA00_SETLOW()
#define NOP() asm volatile ("nop")

static uint8_t ws2812_buffer[WS2812_NUM * 3] = {0};

static void ws2812_send_bytes(uint8_t *data, uint8_t len);


void ws2812_set_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    ws2812_buffer[index*3 + 0] = g;
    ws2812_buffer[index*3 + 1] = r;
    ws2812_buffer[index*3 + 2] = b;
}

void ws2812_sync_display(void)
{
    ws2812_send_bytes(ws2812_buffer, WS2812_NUM * 3);
}



static void ws2812_rest_code(void)
{
    WS2812_PIN_L_LEVEL();
    for (uint8_t i=0; i<200; i++)//复位时间大于50us
    {
        NOP();NOP();NOP();NOP();NOP();
        NOP();NOP();NOP();NOP();NOP();
        NOP();NOP();NOP();NOP();NOP();
        NOP();NOP();NOP();NOP();NOP();
        NOP();NOP();NOP();NOP();NOP();
    }
}

static void ws2812_send_bytes(uint8_t *data, uint8_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        for (uint16_t j = 0; j < 8; j++)
        {
            if( data[i] & (0x80 >> j) )//当前位为1
            {
                WS2812_PIN_H_LEVEL();
                NOP();NOP();NOP();NOP();NOP();
                NOP();NOP();NOP();NOP();NOP();
                NOP();NOP();NOP();NOP();NOP();
                NOP();NOP();NOP();NOP();NOP();
                NOP();
                WS2812_PIN_L_LEVEL();
                // NOP();
            }
            else//当前位为0
            {
                WS2812_PIN_H_LEVEL();
                NOP();NOP();NOP();NOP();NOP();
                NOP();NOP();
                WS2812_PIN_L_LEVEL();
                NOP();NOP();NOP();NOP();NOP();
            }
        }
    }
    ws2812_rest_code();
}






void ws2812_test(void)
{
    ws2812_set_color(0, 255, 0, 0);
    ws2812_set_color(1, 0, 255, 0);
    ws2812_set_color(2, 0, 0, 255);

    ws2812_set_color(3, 255, 0, 0);
    ws2812_set_color(4, 0, 255, 0);
    ws2812_set_color(5, 0, 0, 255);

    ws2812_sync_display();
}


