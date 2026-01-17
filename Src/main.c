/*
 * File: main.c
 * Description: Example implementation with Tx/Rx LED flashing
 */

#include "main.h"
#include "raceup_fdcan.h"

/* --- Private Function Prototypes --- */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void My_CAN_Rx_Callback(uint16_t id, uint8_t* data, uint8_t len);

/* --- Main Application --- */
int main(void)
{
  /* 1. HAL & Low Level Init */
  HAL_Init();

  /* 2. Configure System Clock */
  SystemClock_Config();

  /* 3. Initialize GPIO (PB0 for LED) */
  MX_GPIO_Init();

  /* 4. Initialize FDCAN1 using the RaceUp Wrapper
   * - Baudrate: 500kbps @ 20MHz Kernel
   */
  RU_FDCAN_Init(FDCAN1, FDCAN_NOMINAL_BR_500, FDCAN_REJECT);

  /* 5. Configure Reception Filter (Accept ID 0x123) */
  RU_FDCAN_AddFilter(FDCAN1, FDCAN_FILTER_DUAL, FDCAN_FILTER_TO_RXFIFO0, 0x123, 0x124);
  // RU_FDCAN_AddFilter(FDCAN1, FDCAN_FILTER_RANGE, FDCAN_FILTER_TO_RXFIFO1, 0x126, 0x12A);

  /* 6. Register User Callback */
  RU_FDCAN_RegisterRxFIFO0Callback(FDCAN1, My_CAN_Rx_Callback);

  RU_FDCAN_Start(FDCAN1);

  /* Data to send */
  uint8_t txData[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0x11, 0x22, 0x33, 0x44};

  /* 7. Main Loop */
  while (1)
  {
    /* --- TRANSMIT --- */
    /* Send a message with ID 0x123 (Matches our own filter for loopback test) */
    RU_FDCAN_Send(FDCAN1, 0x123, txData, 8);

    /* FLASH ON TX: Toggle LED to indicate transmission */
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
    HAL_Delay(200);
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
    HAL_Delay(800);
  }
}

/* --- User Callbacks --- */

/**
 * @brief  Called by the wrapper when a valid message is received.
 */
void My_CAN_Rx_Callback(uint16_t id, uint8_t* data, uint8_t len)
{
  /* Check if it is the expected ID */
  // if (id == 0x123)
  // {
      /* FLASH ON RX: Toggle LED to indicate reception */
      HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_4);
  // }
}

/* --- Hardware Configuration --- */

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct_B0 = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* Configure GPIO pin : PB0 (User LED) */
  GPIO_InitStruct_B0.Pin = GPIO_PIN_0;
  GPIO_InitStruct_B0.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_B0.Pull = GPIO_NOPULL;
  GPIO_InitStruct_B0.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_B0);

  GPIO_InitTypeDef GPIO_InitStruct_G4 = {0};
  GPIO_InitStruct_G4.Pin = GPIO_PIN_4;
  GPIO_InitStruct_G4.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_G4.Pull = GPIO_NOPULL;
  GPIO_InitStruct_G4.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct_G4);
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /* Enable HSI (System) and HSE (FDCAN) */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS; 
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  
  /* PLL1 Source HSI */
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 125;
  RCC_OscInitStruct.PLL.PLLP = 4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1);
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
