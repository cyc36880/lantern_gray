#include "color.h"
#include "light.h"
#include "myMath.h"
#include "adc.h"
#include "sys_config_and_flash.h"



typedef struct 
{
    uint8_t num; // 前几个有效
    struct 
    {   
        uint8_t color_pos;
        int diff;
    } color_diff [COLOR_STUDY_NUM];
    
} color2_diff_t;

#define SORT_SIZE 10


static lv_color_hsv_t rgb2hsv(void);
static void update_flash_data(void);

static color2_table_t color2_tab[COLOR_STUDY_NUM] = {0};

static lv_color_hsv_t sort_buf[SORT_SIZE] = {0};
static lv_color_hsv_t median[SORT_SIZE] = {0};
static color2_diff_t color2_diff = {0};



uint8_t color2Val[1] = {0};


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



void color2_init(void)
{
    get_sys_config_data(SYSCONF_MEM_LIST_COLOR_TAB, color2_tab);
}

void color2_study(uint8_t color)
{
    if (color >= COLOR_STUDY_NUM)
        return;
    for (uint8_t i=0; i<SORT_SIZE; i++)
    {
        for (uint8_t j=0; j<SORT_SIZE; j++)
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
    color2_tab[color].hsv.h = median[SORT_SIZE / 2].h;
    color2_tab[color].hsv.s = median[SORT_SIZE / 2].s;
    color2_tab[color].hsv.v = median[SORT_SIZE / 2].v;
    color2_tab[color].has_color = 1;

    update_flash_data();
}


void color2_identify(void)
{
    lv_color_hsv_t hsv = rgb2hsv();

    { // 计算颜色差异
        uint8_t color_diff_count=0;
        for (uint8_t i=0; i<COLOR_STUDY_NUM; i++)
        {
            if (1 == color2_tab[i].has_color)
            {
                color2_diff.color_diff[color_diff_count].diff = \
                    getColorOffset(360, color2_tab[i].hsv.h, hsv.h);
                color2_diff.color_diff[color_diff_count].color_pos= i;
                color_diff_count++;
            }
        }
        color2_diff.num = color_diff_count;
    }
   
    // 没有颜色
    if (color2_diff.num == 0)
    {
        color2Val[0] = 0;
        return;
    }
    // 找到最接近的颜色
    uint8_t min_pos = 0;
    for ( uint8_t i=1; i<color2_diff.num; i++)
    {
        if (color2_diff.color_diff[i].diff < color2_diff.color_diff[min_pos].diff)
        {
            min_pos = i;
        }
    }
    if (color2_diff.color_diff[min_pos].diff > 20 || (hsv.s < 15 && hsv.v < 15)) // 颜色差异过大
    {
        color2Val[0] = 0;
    }
    else
    {
        color2Val[0] = color2_diff.color_diff[min_pos].color_pos+1;
    }
}




void clear_color2_study(void)
{
    memset(color2_tab, 0, sizeof(color2_tab));
    update_flash_data();
}



static void update_flash_data(void)
{
    set_sys_config_info(SYSCONF_MEM_LIST_COLOR_TAB, color2_tab, sizeof(color2_tab));
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








