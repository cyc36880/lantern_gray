#include "color.h"
#include "light.h"
#include "myMath.h"
#include "adc.h"
#include "sys_config_and_flash.h"

#define SORT_SIZE 10

// 色表
static lv_color_hsv_t color_table[COLOR_STUDY_NUM] = { HSV_NONE };

struct COLORBIAS
{
    int16_t bias[sizeof(color_table) / sizeof(color_table[0])];
};


static lv_color_hsv_t rgb2hsv(void);
static void update_flash_data(void);

static lv_color_hsv_t color_buf[sizeof(color_table) / sizeof(color_table[0])][SENSORE_NUM] = {0};
static lv_color_hsv_t sort_buf[SORT_SIZE] = {0};
static lv_color_hsv_t median[SORT_SIZE] = {0};
// 颜色对比偏差
static struct COLORBIAS color_bias[sizeof(color_table) / sizeof(color_table[0])] = {0};

uint8_t colorVal[SENSORE_NUM] = {0};

static void color_hsv_sort_h(lv_color_hsv_t *hsv_buf, uint8_t len)
{
    lv_color_hsv_t hsv;
    for (uint8_t i = 0; i < len - 1; i++)
    {
        for (uint8_t j = i + 1; j < len; j++)
        {
            if (hsv_buf[j].h < hsv_buf[i].h)
            {
                hsv = hsv_buf[i];
                hsv_buf[i] = hsv_buf[j];
                hsv_buf[j] = hsv;
            }
        }
    }
}

static uint16_t getColorOffset(uint16_t max, uint16_t thr, uint16_t cVal)
{
    int16_t val = (thr + cVal) / 2;
    uint16_t val1 = abs(val - thr);
    uint16_t val2 = abs(val - cVal);

    if ((val1 + val2) < max / 2)
        return val1 + val2;
    else
        return max - (val1 + val2);
}

void color_init(void)
{
    get_sys_config_data(SYSCONF_MEM_LIST_COLOR_TAB, color_table);
}

void color_study(uint8_t color)
{
    if (color >= sizeof(color_table) / sizeof(color_table[0]))
        return;
    for (uint8_t port = 0; port < SENSORE_NUM; port++)
    {
        for (uint8_t i = 0; i < SORT_SIZE; i++) // 取十次排序后的中值，并排序
        {
            for (uint8_t j = 0; j < SORT_SIZE; j++) // 取十次数，并排序
            {
                lv_color_hsv_t hsv = rgb2hsv();
                sort_buf[j].h = hsv.h;
                sort_buf[j].s = hsv.s;
                sort_buf[j].v = hsv.v;
            }
            set_rgb_brightness(700, 700, 700); //
            SysTickDelay(15);            // 增加训练的视觉感受
            set_rgb_brightness(0, 0, 0);       //
            SysTickDelay(15);            // 增加训练的视觉感受
            color_hsv_sort_h(sort_buf, SORT_SIZE);
            median[i].h = sort_buf[SORT_SIZE / 2].h;
            median[i].s = sort_buf[SORT_SIZE / 2].s;
            median[i].v = sort_buf[SORT_SIZE / 2].v;
        }
        color_hsv_sort_h(median, SORT_SIZE);
        // color_buf[port][color].h = median[SORT_SIZE / 2].h;
        // color_buf[port][color].s = median[SORT_SIZE / 2].s;
        // color_buf[port][color].v = median[SORT_SIZE / 2].v;

        color_table[color].h = median[SORT_SIZE / 2].h;
    }
    update_flash_data();
}

/**
 * @description: 颜色识别
 * @return {*}
 */
void color_identify(void)
{
    lv_color_hsv_t hsv = rgb2hsv();

    for (int8_t i = 0; i < SENSORE_NUM; i++)
    {
        for (uint8_t j = 0; j < sizeof(color_table) / sizeof(color_table[0]); j++)
        {
            color_bias[i].bias[j] = getColorOffset(360, color_table[j].h, hsv.h);
            // color_bias[i].bias[j] = getColorOffset(360, color_buf[i][j].h, hsv.h);
        }
    }
    // 找出偏差最小的颜色
    for (uint8_t i = 0; i < SENSORE_NUM; i++)
    {
        if (hsv.s < 15 && hsv.v < 15) // 亮度太低，直接跳过
        {
            colorVal[i] = 0;
            continue;
        }

        // 找出偏差最小的颜色
        int8_t min_index = 0;

        for (uint8_t j = 1; j < sizeof(color_buf) / sizeof(color_buf[0]); j++)
        {
            if (color_bias[i].bias[j] < color_bias[i].bias[min_index])
            {
                min_index = j;
            }
        }

        uint8_t n=0;
        for ( ; n < sizeof(color_table) / sizeof(color_table[0]); n++)
        {
            if (color_table[n].h != 0)
                break;
        }
        if (n == sizeof(color_table) / sizeof(color_table[0]))
            colorVal[i] = 0;
        else
            colorVal[i] = min_index + 1;
    }
}

void clear_color_study(void)
{
    memset(color_table, 0, sizeof(color_table));
    update_flash_data();
}



static void update_flash_data(void)
{
    set_sys_config_info(SYSCONF_MEM_LIST_COLOR_TAB, color_table, sizeof(color_table));
    sync_sys_config_info();
}


// =================================
/**
 * @description: r g b 转 hsv
 * @param {uint8_t} r 红灯亮度（0-255）
 * @param {uint8_t} g 绿灯亮度（0-255）
 * @param {uint8_t} b 蓝灯亮度（0-255）
 * @return {*}
 */
static lv_color_hsv_t rgb2hsv(void)
{
    uint8_t r, g, b;

    set_rgb_brightness(600, 0, 0);
    SysTickDelay(1);
    get_adc_val();
    SysTickDelay(1);
    r = VAL4096_2_255(get_adc_val());

    set_rgb_brightness(0, 600, 0);
    SysTickDelay(1);
    get_adc_val();
    SysTickDelay(1);
    g = VAL4096_2_255(get_adc_val());

    set_rgb_brightness(0, 0, 600);
    SysTickDelay(1);
    get_adc_val();
    SysTickDelay(1);
    b = VAL4096_2_255(get_adc_val());

    set_rgb_brightness(0, 0, 0);
    
	return lv_color_rgb_to_hsv(r, g, b);
}



