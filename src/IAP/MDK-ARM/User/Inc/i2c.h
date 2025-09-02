#ifndef __IIC_H__
#define __IIC_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


void MAX_I2C_Init(void);
void MAX_I2C_addr_other_changle(void);
uint8_t MAX_I2C_addr_get_other(void);

#ifdef __cplusplus
}
#endif

#endif /* __IIC_H__ */
