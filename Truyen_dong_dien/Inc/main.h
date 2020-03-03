/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f1xx_hal.h"

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
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SW_Pin GPIO_PIN_15
#define SW_GPIO_Port GPIOC
#define RS_Pin GPIO_PIN_2
#define RS_GPIO_Port GPIOA
#define D_DC_Pin GPIO_PIN_3
#define D_DC_GPIO_Port GPIOA
#define D_CS_Pin GPIO_PIN_4
#define D_CS_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_12
#define CS_GPIO_Port GPIOB
#define DIR_Pin GPIO_PIN_3
#define DIR_GPIO_Port GPIOB
#define STEP_Pin GPIO_PIN_4
#define STEP_GPIO_Port GPIOB
#define MS3_Pin GPIO_PIN_5
#define MS3_GPIO_Port GPIOB
#define INPUT_CAPTURE_Pin GPIO_PIN_6
#define INPUT_CAPTURE_GPIO_Port GPIOB
#define MS2_Pin GPIO_PIN_7
#define MS2_GPIO_Port GPIOB
#define MS1_Pin GPIO_PIN_8
#define MS1_GPIO_Port GPIOB
#define EN_Pin GPIO_PIN_9
#define EN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
