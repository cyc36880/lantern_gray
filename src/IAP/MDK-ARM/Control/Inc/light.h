#ifndef __LIGHT_H__
#define __LIGHT_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * 设置灰度rgb灯
 * 0 - 1000
 */
void set_rgb_brightness(uint16_t red, uint16_t green, uint16_t blue);
void set_light_brightness(uint16_t red, uint16_t green, uint16_t blue);

#ifdef __cplusplus
}
#endif


#endif /* __LIGHT_H__ */
