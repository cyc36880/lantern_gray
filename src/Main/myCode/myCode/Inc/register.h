#ifndef __REGISTER_H__
#define __REGISTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef struct
{
    uint8_t* reg;
    uint8_t size;
    uint8_t changle_flag;
} Register_t;

extern Register_t sys_register;
extern Register_t ultr_register;

#ifdef __cplusplus
}
#endif

#endif /* __REGISTER_H__ */
