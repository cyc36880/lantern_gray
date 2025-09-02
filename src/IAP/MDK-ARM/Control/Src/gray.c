#include "gray.h"
#include "sys_config_and_flash.h"
#include "light.h"
#include "myMath.h"
#include "string.h"
#include "adc.h"

Grayscale_Port Grayscale_Port_param[1] = {
    {.maxVal=4095, .minVal=0, .MedianVal=2047, .zoom=1.0, .offset=0},
};

static int32_t Grayscale[SENSORE_NUM] = {0};
// 识别后的n路灰度值
uint8_t grayVal[SENSORE_NUM] = {0};


void Grayscale_Init(void)
{
    get_sys_config_data(SYSCONF_MEM_LIST_GRAY_PARAM, Grayscale_Port_param);
}


//  写灰度学习的值
static void update_Grayscale_flashdata(void)
{
    Grayscale_Port Grayscale_param[1];

    memcpy(Grayscale_param, Grayscale_Port_param, sizeof(Grayscale_Port));
    set_sys_config_info(SYSCONF_MEM_LIST_GRAY_PARAM, Grayscale_param, SENSORE_NUM*sizeof(Grayscale_Port));
    sync_sys_config_info();
}


/**
 * @description: 灰度学习（死循环学习）
 * @param {uint8_t} r
 * @param {uint8_t} g
 * @param {uint8_t} b
 * @return {*}
 */
void gary_study(void)
{
    for(uint8_t i=0; i<SENSORE_NUM; i++)
    {
        Grayscale_Port_param[i].maxVal = 0;
        Grayscale_Port_param[i].minVal = 4095;
    }

    uint32_t tickstart = GetTick();
    uint32_t wait = 1000 * 5;
    while ((GetTick() - tickstart) < wait) //
    {
        set_rgb_brightness(1000, 1000, 1000);
        SysTickDelay(1);
        for(uint8_t i = 0; i < SENSORE_NUM; i++)
        {
            uint16_t adc_val = get_adc_val();
            if ( adc_val > Grayscale_Port_param[i].maxVal )
                Grayscale_Port_param[i].maxVal = adc_val;
            if ( adc_val < Grayscale_Port_param[i].minVal )
                Grayscale_Port_param[i].minVal = adc_val;
        }

        SysTickDelay(15); //增加训练的视觉感受
        set_rgb_brightness(0, 0, 0); //
        SysTickDelay(15); //增加训练的视觉感受
    }
    for(uint8_t i = 0; i < SENSORE_NUM; i++)
    {
        Grayscale_Port_param[i].zoom = 4095.0f / (Grayscale_Port_param[i].maxVal - Grayscale_Port_param[i].minVal); //计算缩放比例
        Grayscale_Port_param[i].MedianVal = (Grayscale_Port_param[i].maxVal + Grayscale_Port_param[i].minVal) / 2; // 计算中值
        Grayscale_Port_param[i].offset = Grayscale_Port_param[i].minVal;
    }
    update_Grayscale_flashdata(); //向flash写入数据
}

void gary_identify(void)
{
    set_rgb_brightness(1000, 1000, 1000);
    SysTickDelay(1);//adc转换时间
    for(uint8_t i = 0; i < SENSORE_NUM; i++) //获取原始值
    {
        Grayscale[i] = get_adc_val();   
    }

    for(uint8_t i = 0; i < SENSORE_NUM; i++) 
    {
        Grayscale[i] +=  (Grayscale_Port_param[i].MedianVal - Grayscale[i])*2;//围绕中值反转
        Grayscale[i] = (Grayscale[i] - Grayscale_Port_param[i].offset) * Grayscale_Port_param[i].zoom; //比例缩放
    }
    
    for(uint8_t i = 0; i < SENSORE_NUM; i++) // 输出灰度值
    {
        if(Grayscale[i] < 0) Grayscale[i] = 0;
        Grayscale[i] = VAL4096_2_255(Grayscale[i]);

        Grayscale[i] = Grayscale[i]<=255 ? Grayscale[i] : 255;//放大限幅
        Grayscale[i] = Grayscale[i]>=0 ? Grayscale[i] : 0;
        grayVal[i] = Grayscale[i];
    }
}


