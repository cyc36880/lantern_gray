#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


typedef struct 
{
    void* Instance;
} ADC_HandleTypeDef;





void MAX_ADC_Init(void);
uint16_t get_adc_val(void);



#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
