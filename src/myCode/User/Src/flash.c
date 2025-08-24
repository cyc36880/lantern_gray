#include "flash.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define FLASH_BUFFER_SIZE 200
#define FLASH_PAGE        127
#define FLASH_PAGE_SIZE   512
#define FLASH_START_ADDR  (0x0 + FLASH_PAGE*FLASH_PAGE_SIZE)

static uint8_t flash_buffer[FLASH_BUFFER_SIZE] = {0};
/**
 * page 0-127
 * 
 * LOCK 0 - 15
 * 
 * LOCK15: Page120 - Page127
 * 
 */

void HAL_Flash_Init(void)
{

}

int HAL_Flash_Read(uint32_t addr, uint8_t *data, uint32_t size)
{
    uint32_t start_addr = FLASH_START_ADDR+addr;
    for (uint32_t i=0; i<size; i++)
    {
        data[i] = *(uint8_t *)(start_addr+i);
        flash_buffer[addr+i] = data[i];
    }
    return 0;
}

int HAL_Flash_Write(uint32_t addr, uint8_t *data, uint32_t size)
{
    for (uint32_t i=0; i<size; i++)
    {
        flash_buffer[addr+i] = data[i];
    }
    return 0;
}

int HAL_Flash_Sync(void)
{
    FLASH_UnlockPages(FLASH_START_ADDR, FLASH_START_ADDR);
    FLASH_ErasePages(FLASH_START_ADDR, FLASH_START_ADDR);
    FLASH_LockAllPages();

    SysTickDelay(1);

    FLASH_UnlockPages(FLASH_START_ADDR, FLASH_START_ADDR);
    FLASH_WriteBytes(FLASH_START_ADDR, flash_buffer, FLASH_BUFFER_SIZE);
    FLASH_LockAllPages();

    return 0;
}























