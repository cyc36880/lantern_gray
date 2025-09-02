#include "light.h"
#include "myMath.h"

/**
 * 设置灰度rgb灯
 * 0 - 1000
 */
void set_rgb_brightness(uint16_t red, uint16_t green, uint16_t blue)
{
    red   = 1000 - max(0, min(1000, red));
    green = 1000 - max(0, min(1000, green));
    blue  = 1000 - max(0, min(1000, blue));

    ATIM_SetCompare1(red);
    ATIM_SetCompare2(green);
    GTIM_SetCompare4(CW_GTIM1, blue);
}

void set_light_brightness(uint16_t red, uint16_t green, uint16_t blue)
{
    red   = 1000 - max(0, min(1000, red));
    green = 1000 - max(0, min(1000, green));
    blue  = 1000 - max(0, min(1000, blue));

    GTIM_SetCompare3(CW_GTIM1, red);
    GTIM_SetCompare2(CW_GTIM1, green);
    GTIM_SetCompare1(CW_GTIM1, blue);
}
