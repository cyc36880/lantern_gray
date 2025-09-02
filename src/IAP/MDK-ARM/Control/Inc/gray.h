#ifndef __GRAY_H__
#define __GRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


typedef struct 
{
   int32_t maxVal;
   int32_t minVal;
   int32_t MedianVal;
   float zoom;
   int32_t offset;
}Grayscale_Port;


void Grayscale_Init(void);
void gary_study(void);
void gary_identify(void);

extern uint8_t grayVal[SENSORE_NUM];

#ifdef __cplusplus
}
#endif

#endif /* __GRAY_H__ */
