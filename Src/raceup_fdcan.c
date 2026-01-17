/*
 * File: raceup_fdcan.c
 */

#include "raceup_fdcan.h"
#include <string.h> /* For memcpy */

/* Global Instances */
RU_FDCAN_HandleTypeDef hRUCAN1;
#ifdef FDCAN2
RU_FDCAN_HandleTypeDef hRUCAN2;
#endif

/* Private Helper to get wrapper handle from instance */
static RU_FDCAN_HandleTypeDef* GetHandle(FDCAN_GlobalTypeDef *Instance) {
    if (Instance == FDCAN1) return &hRUCAN1;
#ifdef FDCAN2
    if (Instance == FDCAN2) return &hRUCAN2;
#endif
    return NULL;
}

/* Private helper to convert byte length to HAL DLC code */
static uint32_t Get_HAL_DLC(uint8_t len) {
    if (len > 8) len = 8; /* Constraint: Max 8 bytes */
    switch(len) {
        case 0: return FDCAN_DLC_BYTES_0;
        case 1: return FDCAN_DLC_BYTES_1;
        case 2: return FDCAN_DLC_BYTES_2;
        case 3: return FDCAN_DLC_BYTES_3;
        case 4: return FDCAN_DLC_BYTES_4;
        case 5: return FDCAN_DLC_BYTES_5;
        case 6: return FDCAN_DLC_BYTES_6;
        case 7: return FDCAN_DLC_BYTES_7;
        case 8: return FDCAN_DLC_BYTES_8;
        default: return FDCAN_DLC_BYTES_8;
    }
}

/* Private helper to convert HAL DLC code back to bytes */
static uint8_t Get_Len_From_DLC(uint32_t dlc) {
    switch(dlc) {
        case FDCAN_DLC_BYTES_0: return 0;
        case FDCAN_DLC_BYTES_1: return 1;
        case FDCAN_DLC_BYTES_2: return 2;
        case FDCAN_DLC_BYTES_3: return 3;
        case FDCAN_DLC_BYTES_4: return 4;
        case FDCAN_DLC_BYTES_5: return 5;
        case FDCAN_DLC_BYTES_6: return 6;
        case FDCAN_DLC_BYTES_7: return 7;
        case FDCAN_DLC_BYTES_8: return 8;
        default: return 8;
    }
}

void RU_FDCAN_Init(FDCAN_GlobalTypeDef *Instance, RU_FDCAN_NominalBitrate br, uint32_t global_filter) {
  RU_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
  if (!hWrapper) return;

  hWrapper->hfdcan.Instance = Instance;
  hWrapper->hfdcan.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  
  /* Constraint: Classic CAN Frame (No BRS, No Wide Data) */
  hWrapper->hfdcan.Init.FrameFormat = FDCAN_FRAME_CLASSIC; 
  
  hWrapper->hfdcan.Init.Mode = FDCAN_MODE_NORMAL;
  hWrapper->hfdcan.Init.AutoRetransmission = ENABLE;
  hWrapper->hfdcan.Init.TransmitPause = DISABLE;
  hWrapper->hfdcan.Init.ProtocolException = DISABLE;

  uint32_t presc, ts1, ts2, sjw;
  switch (br) {
    case FDCAN_NOMINAL_BR_125:
      presc = 10;
      ts1 = 13;
      ts2 = 2;
      sjw = 1;
      break;

    case FDCAN_NOMINAL_BR_250:
      presc = 5;
      ts1 = 13;
      ts2 = 2;
      sjw = 1;
      break;
    case FDCAN_NOMINAL_BR_1000:
      presc = 2;
      ts1 = 8;
      ts2 = 1;
      sjw = 1;
      break;
    case FDCAN_NOMINAL_BR_500:
    default:
      presc = 4;
      ts1 = 8;
      ts2 = 1;
      sjw = 1;
  }
  /* Nominal Bit Timing (Arbitration) */
  hWrapper->hfdcan.Init.NominalPrescaler = presc;
  hWrapper->hfdcan.Init.NominalSyncJumpWidth = sjw;
  hWrapper->hfdcan.Init.NominalTimeSeg1 = ts1;
  hWrapper->hfdcan.Init.NominalTimeSeg2 = ts2;

  /* Data Bit Timing (Same as Nominal since BRS is OFF) */
  hWrapper->hfdcan.Init.DataPrescaler = presc;
  hWrapper->hfdcan.Init.DataSyncJumpWidth = sjw;
  hWrapper->hfdcan.Init.DataTimeSeg1 = ts1;
  hWrapper->hfdcan.Init.DataTimeSeg2 = ts2;

  hWrapper->hfdcan.Init.StdFiltersNbr = 28;
  hWrapper->hfdcan.Init.ExtFiltersNbr = 0;
  hWrapper->hfdcan.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;

  if (HAL_FDCAN_Init(&hWrapper->hfdcan) != HAL_OK) {
      while(1); /* Error trap */
  }

  if (HAL_FDCAN_ConfigGlobalFilter(&hWrapper->hfdcan,
                                   global_filter,
                                   global_filter,
                                   FDCAN_REJECT_REMOTE,
                                   FDCAN_REJECT_REMOTE) != HAL_OK)
  {
    while(1);
  }

  HAL_FDCAN_ConfigInterruptLines(&hWrapper->hfdcan,
                                 FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO1_NEW_MESSAGE | FDCAN_IT_RX_HIGH_PRIORITY_MSG,
                                 FDCAN_INTERRUPT_LINE0);

  HAL_FDCAN_ConfigInterruptLines(&hWrapper->hfdcan,
                                 FDCAN_IT_BUS_OFF | FDCAN_IT_ERROR_WARNING | FDCAN_IT_ERROR_PASSIVE,
                                 FDCAN_INTERRUPT_LINE1);

  /* Activate Interrupts for RX FIFO0 */
  HAL_FDCAN_ActivateNotification(&hWrapper->hfdcan,
                                 FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO1_NEW_MESSAGE | FDCAN_IT_RX_HIGH_PRIORITY_MSG,
                                 0);
  HAL_FDCAN_ActivateNotification(&hWrapper->hfdcan,
                                 FDCAN_IT_BUS_OFF | FDCAN_IT_ERROR_WARNING | FDCAN_IT_ERROR_PASSIVE,
                                 0);
  
  hWrapper->Initialized = 1;
}

void RU_FDCAN_Start(FDCAN_GlobalTypeDef *Instance) {
    RU_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (!hWrapper) return;
    
    HAL_FDCAN_Start(&hWrapper->hfdcan);
}

void RU_FDCAN_RegisterRxFIFO0Callback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(uint16_t id, uint8_t* data, uint8_t len)) {
    RU_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (hWrapper) {
        hWrapper->RxFIFO0Callback = Callback;
    }
}

void RU_FDCAN_RegisterRxFIFO1Callback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(uint16_t id, uint8_t* data, uint8_t len)) {
    RU_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (hWrapper) {
        hWrapper->RxFIFO1Callback = Callback;
    }
}

void RU_FDCAN_RegisterErrorCallback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(uint32_t error_flags))
{
  RU_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
  if (hWrapper) {
    hWrapper->ErrorCallback = Callback;
  }
}

void RU_FDCAN_RegisterHpCallback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(FDCAN_HpMsgStatusTypeDef* hpStatus)) {
  RU_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
  if (hWrapper) {
    hWrapper->HpCallback = Callback;
  }
}

void RU_FDCAN_AddFilter(FDCAN_GlobalTypeDef *Instance, uint32_t type, uint32_t config, uint16_t id1, uint16_t id2) {
    RU_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (!hWrapper) return;

    /* We need to temporarily stop to add filters safely if already running, 
       but HAL_FDCAN_ConfigFilter generally works in config mode. 
       STM32H5 HAL handles context switching usually, but let's be safe. */
    
    FDCAN_FilterTypeDef sFilterConfig;
    static uint32_t filterIdx1 = 0;
    static uint32_t filterIdx2 = 0;

    sFilterConfig.IdType = FDCAN_STANDARD_ID;
    sFilterConfig.FilterType = type;
    sFilterConfig.FilterConfig = config;
    sFilterConfig.FilterID1 = id1;
    sFilterConfig.FilterID2 = id2; /* 0x7FF for exact match */
    
    if(Instance == FDCAN1) {
        sFilterConfig.FilterIndex = filterIdx1++;
    } 
#ifdef FDCAN2
    else if(Instance == FDCAN2) {
        sFilterConfig.FilterIndex = filterIdx2++;
    }
#endif

  if (HAL_FDCAN_ConfigFilter(&hWrapper->hfdcan, &sFilterConfig) != HAL_OK) {
    while(1);
  }
    
}

void RU_FDCAN_Send(FDCAN_GlobalTypeDef *Instance, uint16_t id, uint8_t* data, uint8_t len) {
    RU_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (!hWrapper) return;

    FDCAN_TxHeaderTypeDef TxHeader;
    
    TxHeader.Identifier = id;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = Get_HAL_DLC(len);
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF; /* Constraint: No BRS */
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;  /* Constraint: Classic Format */
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    HAL_FDCAN_AddMessageToTxFifoQ(&hWrapper->hfdcan, &TxHeader, data);
}

/* * Bridge function to be called from stm32h5xx_it.c 
 * It resolves the Instance (FDCAN1/2) to the hidden internal HAL handle.
 */
void RU_FDCAN_IRQHandler(FDCAN_GlobalTypeDef *Instance) {
    RU_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    
    if (hWrapper != NULL) {
        /* Let the standard HAL handler do the heavy lifting:
         * 1. Check which interrupt fired (RX, TX, Error).
         * 2. Clear the interrupt flags.
         * 3. Call the associated Callback function.
         */
        HAL_FDCAN_IRQHandler(&hWrapper->hfdcan);
    }
}

/*
 * HAL Callback Override
 * This is called automatically by HAL_FDCAN_IRQHandler when a message arrives.
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
  RU_FDCAN_HandleTypeDef *targetWrapper = GetHandle(hfdcan->Instance);

    /* 2. Execute Custom Logic */
    if (targetWrapper && targetWrapper->RxFIFO0Callback) {
        /* Check if the interrupt is actually for a New Message */
        if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0) {
            FDCAN_RxHeaderTypeDef RxHeader;
            uint8_t RxData[8];

            /* Retrieve the message using HAL */
            if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
                /* Pass it to the user's registered callback */
                targetWrapper->RxFIFO0Callback((uint16_t)RxHeader.Identifier, RxData, Get_Len_From_DLC(RxHeader.DataLength));
            }
        }
    }
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs) {
  RU_FDCAN_HandleTypeDef *targetWrapper = GetHandle(hfdcan->Instance);

    /* 2. Execute Custom Logic */
    if (targetWrapper && targetWrapper->RxFIFO1Callback) {
        /* Check if the interrupt is actually for a New Message */
        if ((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != 0) {
            FDCAN_RxHeaderTypeDef RxHeader;
            uint8_t RxData[8];

            /* Retrieve the message using HAL */
            if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &RxHeader, RxData) == HAL_OK) {
                /* Pass it to the user's registered callback */
                targetWrapper->RxFIFO1Callback((uint16_t)RxHeader.Identifier, RxData, Get_Len_From_DLC(RxHeader.DataLength));
            }
        }
    }
}

void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
  RU_FDCAN_HandleTypeDef *targetWrapper = GetHandle(hfdcan->Instance);

  if (targetWrapper && targetWrapper->ErrorCallback) {
    targetWrapper->ErrorCallback(ErrorStatusITs);
  }
}

void HAL_FDCAN_HighPriorityMessageCallback(FDCAN_HandleTypeDef *hfdcan)
{
  RU_FDCAN_HandleTypeDef *targetWrapper = GetHandle(hfdcan->Instance);

  if (targetWrapper && targetWrapper->HpCallback) {
    FDCAN_HpMsgStatusTypeDef hpStatus;

    if (HAL_FDCAN_GetHighPriorityMessageStatus(hfdcan, &hpStatus) == HAL_OK) {
      targetWrapper->HpCallback(&hpStatus);
    }
  }
}


/* * MSP Initialization (Clock & Pin Mux)
 * This must be implemented to support the Init function.
 * You might need to adjust pins based on your specific board layout.
 */
void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* hfdcan) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    if(hfdcan->Instance == FDCAN1) {
        /* FDCAN1 Clock Config */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
        PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL2Q;
        PeriphClkInitStruct.PLL2.PLL2Source = RCC_PLL2_SOURCE_HSE; 
        PeriphClkInitStruct.PLL2.PLL2M = 1;
        PeriphClkInitStruct.PLL2.PLL2N = 50;  // 400MHz
        PeriphClkInitStruct.PLL2.PLL2P = 2;
        PeriphClkInitStruct.PLL2.PLL2Q = 20;  // 20MHz Kernel
        PeriphClkInitStruct.PLL2.PLL2R = 2;
        PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2_VCIRANGE_3;
        PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2_VCORANGE_MEDIUM;
        PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
        PeriphClkInitStruct.PLL2.PLL2ClockOut = RCC_PLL2_DIVQ;
        
        HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
        
        __HAL_RCC_FDCAN_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        
        /* FDCAN1 GPIO: PA11/PA12 */
        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        /* Interrupt */
        HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);

        HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 6, 0);
        HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
    }
#ifdef FDCAN2
    else if(hfdcan->Instance == FDCAN2) {
        /* FDCAN2 uses the same FDCAN Clock Source (PLL2Q), just enable the IP clock */
        __HAL_RCC_FDCAN_CLK_ENABLE(); 
        __HAL_RCC_GPIOB_CLK_ENABLE(); /* Example: PB12/PB13 for FDCAN2 */

        GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* Interrupt */
        HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);

        HAL_NVIC_SetPriority(FDCAN2_IT1_IRQn, 6, 0);
        HAL_NVIC_EnableIRQ(FDCAN2_IT1_IRQn);
    }
#endif
}
