#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


void HAL_Flash_Init(void);
int HAL_Flash_Write(uint32_t addr, uint8_t *data, uint32_t size);
int HAL_Flash_Read(uint32_t addr, uint8_t *data, uint32_t size);
int HAL_Flash_Sync(void);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_H__ */
