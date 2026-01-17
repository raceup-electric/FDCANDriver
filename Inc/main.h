/*
 * File: main.h
 * Description: Header for main.c file.
 * This file contains the common defines of the application.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
/* We define these to make changing pins easier later, matching your main.c logic */
#define USER_LED_Pin       GPIO_PIN_0
#define USER_LED_GPIO_Port GPIOB

/* FDCAN Pins (Reference only, these are set in MspInit) */
#define FDCAN1_RX_Pin      GPIO_PIN_11
#define FDCAN1_RX_GPIO_Port GPIOA
#define FDCAN1_TX_Pin      GPIO_PIN_12
#define FDCAN1_TX_GPIO_Port GPIOA

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
