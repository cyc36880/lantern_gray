#ifndef __WS2812_H__
#define __WS2812_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "../../myCode/myMain.h"

#define WS2812_NUM  6

void ws2812_set_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void ws2812_sync_display(void);


#ifdef __cplusplus
}
#endif


#endif /* __WS2812_H__ */

