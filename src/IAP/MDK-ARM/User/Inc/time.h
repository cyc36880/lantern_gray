#ifndef __TIME_H__
#define __TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef struct 
{
    void* xtime_Instance;
} TIME_HandleTypeDef;

void MAX_ATIME_Init(void);
void MAX_GTIME_Init(void);


#ifdef __cplusplus
}
#endif

#endif /* __TIME_H__ */
