#ifndef __COLOR_H__
#define __COLOR_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"
#include "myMath.h"

typedef struct 
{
    uint8_t has_color; // 是否有颜色 1有 0没有
    lv_color_hsv_t hsv;
} color2_table_t;


#define COLOR_STUDY_NUM  8

// 色卡
#define HSV_RED       {0, 57, 5}    //红 1
#define HSV_GREEN     {135, 70, 7}  //绿 2
#define HSV_BLUE      {210, 90, 10} //蓝 3
#define HSV_YELLOW    {88, 60, 10}  //黄 4
#define HSV_CYAN      {190, 75, 10} //青 5
#define HSV_PURPLE    {222, 69, 5}  //紫 6
#define HSV_BLACK     {0, 0, 0}     //黑 7
#define HSV_WHITE     {198, 44, 13} //白 8

#define HSV_NONE      {0, 0, 0}     //无

// extern uint8_t colorVal[SENSORE_NUM];

// void color_init(void);
// void color_study(uint8_t color);
// void color_identify(void);
// void clear_color_study(void);




// 2
void color2_init(void);
void color2_study(uint8_t color);
void color2_identify(void);
void clear_color2_study(void);

extern uint8_t color2Val[1];


#ifdef __cplusplus
}
#endif

#endif /* __COLOR_H__ */

