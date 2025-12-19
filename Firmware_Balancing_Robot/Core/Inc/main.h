/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI_CS_Pin GPIO_PIN_1
#define SPI_CS_GPIO_Port GPIOC
#define RGB_R_Pin GPIO_PIN_0
#define RGB_R_GPIO_Port GPIOB
#define RGB_G_Pin GPIO_PIN_1
#define RGB_G_GPIO_Port GPIOB
#define RGB_B_Pin GPIO_PIN_2
#define RGB_B_GPIO_Port GPIOB
#define RC_CH2_Pin GPIO_PIN_14
#define RC_CH2_GPIO_Port GPIOB
#define STSP_RST_Pin GPIO_PIN_8
#define STSP_RST_GPIO_Port GPIOC
#define PHA_Pin GPIO_PIN_9
#define PHA_GPIO_Port GPIOC
#define PWMA_Pin GPIO_PIN_8
#define PWMA_GPIO_Port GPIOA
#define PWMB_Pin GPIO_PIN_9
#define PWMB_GPIO_Port GPIOA
#define PHB_Pin GPIO_PIN_10
#define PHB_GPIO_Port GPIOA
#define ENTER_Pin GPIO_PIN_11
#define ENTER_GPIO_Port GPIOA
#define MODE_Pin GPIO_PIN_12
#define MODE_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
