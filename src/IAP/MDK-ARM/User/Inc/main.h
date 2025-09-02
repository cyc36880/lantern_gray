/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 CW.
 * All rights reserved.</center></h2>
 *
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "cw32l010_hal.h"

  /* Private includes ----------------------------------------------------------*/
  /* USER CODE BEGIN Includes */
  /* USER CODE END Includes */

  /* Exported types ------------------------------------------------------------*/
  /* USER CODE BEGIN ET */
  /* USER CODE END ET */

  /* Exported constants --------------------------------------------------------*/
  /* USER CODE BEGIN EC */
  /* USER CODE END EC */

  /* Exported macro ------------------------------------------------------------*/
  /* USER CODE BEGIN EM */
  /* USER CODE END EM */

  /* Exported functions prototypes ---------------------------------------------*/
  /* USER CODE BEGIN EFP */
  /* USER CODE END EFP */

  /* Private defines -----------------------------------------------------------*/
  /* USER CODE BEGIN Private defines */

#define SENSORE_NUM 1 // 传感器路数

#define KEY_PIN GPIO_PIN_3
#define KEY_GPIO_Port CW_GPIOA

#define FLASH_BLOCK_SIZE 512

#define FLASH_APP_ADDR 0x2800 // Application start address
#define FLASH_APP_SIZE (27 * 1024)

#define FLASH_SAVE_ADDR 0x9400 // 固件保存地址
#define FLASH_SAVE_SIZE (27 * 1024 - 512)

#define FLASH_INFO_ADDR 0xFE00 // 保存数据地址
#define FLASH_INFO_SIZE (512)

  /* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT CW *****END OF FILE****/
