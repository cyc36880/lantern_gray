#include "flash.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FLASH_BUFFER_SIZE 128

typedef void (*pFunction)(void);

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
    uint32_t start_addr = FLASH_INFO_ADDR + addr;
    for (uint32_t i = 0; i < size; i++)
    {
        data[i] = *(uint8_t *)(start_addr + i);
        flash_buffer[addr + i] = data[i];
    }
    return 0;
}

int HAL_Flash_Write(uint32_t addr, uint8_t *data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        flash_buffer[addr + i] = data[i];
    }
    return 0;
}

int HAL_Flash_Sync(void)
{
    FLASH_UnlockPages(FLASH_INFO_ADDR, FLASH_INFO_ADDR);
    FLASH_ErasePages(FLASH_INFO_ADDR, FLASH_INFO_ADDR);
    FLASH_LockAllPages();

    SysTickDelay(1);

    FLASH_UnlockPages(FLASH_INFO_ADDR, FLASH_INFO_ADDR);
    FLASH_WriteBytes(FLASH_INFO_ADDR, flash_buffer, FLASH_BUFFER_SIZE);
    FLASH_LockAllPages();

    return 0;
}

void jump(uint32_t APPLICATION_ADDRESS)
{
    pFunction JumpToApplication;
    uint32_t JumpAddress;
    __disable_irq(); // 关中断

    SCB->VTOR = APPLICATION_ADDRESS; // 设置中断向量偏移地址
    /* execute the new program */
    JumpAddress = *(__IO uint32_t *)(APPLICATION_ADDRESS + 4);
    /* Jump to user application */
    JumpToApplication = (pFunction)JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t *)APPLICATION_ADDRESS);
    JumpToApplication();
}

static uint8_t flash_copy_buffer[FLASH_BLOCK_SIZE] = {0};

int Flash_copy(uint32_t target_addr, uint32_t source_addr, uint32_t size)
{
    uint32_t num_blocks = (size + FLASH_BLOCK_SIZE - 1) / FLASH_BLOCK_SIZE;

    FLASH_UnlockPages(target_addr, target_addr + size - 1);
    FLASH_ErasePages(target_addr, target_addr + size - 1);
    SysTickDelay(1);

    for (uint32_t i = 0; i < num_blocks; i++)
    {
        uint32_t offset = i * FLASH_BLOCK_SIZE;
        uint32_t remain = size - offset;
        uint32_t copy_len = (remain >= FLASH_BLOCK_SIZE) ? FLASH_BLOCK_SIZE : remain;

        memset(flash_copy_buffer, 0xFF, FLASH_BLOCK_SIZE);
        memcpy(flash_copy_buffer, (uint8_t *)(source_addr + offset), copy_len);

        if (0 != FLASH_WriteBytes(target_addr + offset, flash_copy_buffer, FLASH_BLOCK_SIZE))
        {
            FLASH_LockAllPages();
            return -2;
        }
    }
    FLASH_LockAllPages();

    return 0;
}
