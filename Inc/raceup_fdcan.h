/**
 * ******************************************************************************
 * @file    raceup_fdcan.h
 * @author  Luca Domeneghetti
 * @brief   Header file for the RaceUp FDCAN driver.
 *
 * @details This file contains the function prototypes, definitions, and macros
 * used to abstract the STM32 HAL FDCAN functionalities. It provides
 * interfaces for initialization, filter configuration, and data transmission.
 *
 *
 * ******************************************************************************
 */
#ifndef _RACEUP_FDCAN_H
#define _RACEUP_FDCAN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/**
 * @addtogroup RaceUp_Drivers RaceUp Drivers
 * @brief Base group of RaceUp Drivers
 * @{
 */

/**
 * @defgroup RaceUp_FDCAN FDCAN Driver
 * @brief FDCAN Driver module for handling CAN FD communication.
 * @details This module provides a simplified API for configuring and utilizing the FDCAN peripheral.
 * In particular, it allows for the configuration of transmission bit rate, reception filters, transmission and reception of messages.
 * Incoming FDCAN IRQ signals can be assigned and handled by dedicated callback functions, created by the user.
 * This module leverages the STM32 HAL FDCAN library.
 * @par Example Usage
 *
 * The following example demonstrates how to initialize FDCAN1, register a callback
 * for received messages, and send a data frame.
 *
 * @code
 * /* --- 1. Define User Callbacks --- */
 * void My_RxCallback(uint16_t id, uint8_t* data, uint8_t len)
 * {
 * // Handle received data (e.g., parse telemetry)
 * if (id == 0x200) {
 * // Do something
 * }
 * }
 * * void My_ErrorCallback(uint32_t error_flags)
 * {
 * // Handle bus errors
 * }
 *
 * /* --- 2. Initialization Sequence --- */
 * void App_CAN_Init(void)
 * {
 * /* Initialize FDCAN1 at 500 kbit/s. 
 * * Non-matching frames are REJECTED (not sent to FIFO). */
 * RU_FDCAN_Init(FDCAN1, FDCAN_NOMINAL_BR_500, FDCAN_REJECT);
 *
 * /* Register the Callbacks */
 * RU_FDCAN_RegisterRxFIFO0Callback(FDCAN1, My_RxCallback);
 * RU_FDCAN_RegisterErrorCallback(FDCAN1, My_ErrorCallback);
 *
 * /* Add a Filter: Accept Standard ID 0x200 into RX FIFO 0 */
 * /* Mask 0x7FF means exact match for 11-bit ID */
 * RU_FDCAN_AddFilter(FDCAN1, FDCAN_FILTER_MASK, FDCAN_FILTER_TO_RXFIFO0, 0x200, 0x7FF);
 *
 * /* Start the Peripheral and Enable Interrupts */
 * RU_FDCAN_Start(FDCAN1);
 * }
 *
 * /* --- 3. Sending Data --- */
 * void App_Send_Telemetry(void)
 * {
 * uint8_t payload[4] = {0xDE, 0xAD, 0xBE, 0xEF};
 * * /* Send Standard ID 0x100 with 4 bytes of data */
 * RU_FDCAN_Send(FDCAN1, 0x100, payload, 4);
 * }
 * @endcode
 * @{
 */

/* Exported constants ---------------------------------------------------------------------*/
/**
 * @defgroup RaceUp_FDCAN_Exported_Constants FDCAN Exported Constants
 * @{
 */ 
#define RACEUP_FDCAN_DLC_MAX      8U
#define RUP_FDCAN_TIMEOUT         1000U
/**
 * @}
 */

/* Exported macros ---------------------------------------------------------------------*/
/**
 * @defgroup RaceUp_FDCAN_Exported_Macros FDCAN Exported Macros
 * @{
 */ 

/**
 * @}
 */

/* Exported types ---------------------------------------------------------------------*/
/**
 * @defgroup RaceUp_FDCAN_Exported_Macros FDCAN Exported Macros
 * @{
 */ 

/**
 * @brief   RaceUp FDCAN Handle Structure.
 * @details Extends the HAL handle to include application-specific callbacks
 * and error tracking.
 */
typedef struct {
    FDCAN_HandleTypeDef hfdcan;     /*!< Low-level HAL FDCAN Handle */
    
    /* User Callbacks */
    void (*RxFIFO0Callback)(uint16_t id, uint8_t* data, uint8_t len); /*!< Pointer to RX FIFO 0 callback */
    void (*RxFIFO1Callback)(uint16_t id, uint8_t* data, uint8_t len); /*!< Pointer to RX FIFO 1 callback */
    void (*ErrorCallback)(uint32_t error_flags);                      /*!< Pointer to Error callback */
    void (*HpCallback)(FDCAN_HpMsgStatusTypeDef* hpStatus);           /*!< Pointer to High Priority Msg callback */

    volatile uint8_t Initialized;   /*!< Driver initialization state */

} RU_FDCAN_HandleTypeDef;

/**
 * @brief   RaceUp FDCAN Status Enumeration.
 * @details Used to return the status of driver operations.
 */
typedef enum {
    RU_FDCAN_OK       = 0x00U, /*!< Operation completed successfully */
    RU_FDCAN_ERROR    = 0x01U, /*!< Operation failed */
    RU_FDCAN_BUSY     = 0x02U, /*!< Driver is busy */
    RU_FDCAN_TIMEOUT  = 0x03U  /*!< Operation timed out */
} RU_FDCAN_StatusTypeDef;

/**
 * @brief   RaceUp FDCAN Nominal Bitrate settings.
 * @details Enumeration of standard nominal bitrates supported by the application.
 * These identifiers can be mapped to specific Prescaler/TimeSeg settings
 * in the initialization logic.
 */
typedef enum {
  FDCAN_NOMINAL_BR_125,   /*!< Nominal Bitrate: 125 kbit/s */
  FDCAN_NOMINAL_BR_250,   /*!< Nominal Bitrate: 250 kbit/s */
  FDCAN_NOMINAL_BR_500,   /*!< Nominal Bitrate: 500 kbit/s */
  FDCAN_NOMINAL_BR_1000   /*!< Nominal Bitrate: 1 Mbit/s   */
} RU_FDCAN_NominalBitrate;
/**
 * @}
 */

/* Exported variables --------------------------------------------------------*/
/** @defgroup RaceUp_FDCAN_Exported_Variables FDCAN Exported Variables
 * @{
 */

extern RU_FDCAN_HandleTypeDef hRUCAN1; /*!< Global handle for FDCAN1 */

#ifdef FDCAN2
extern RU_FDCAN_HandleTypeDef hRUCAN2; /*!< Global handle for FDCAN2 (if available) */
#endif

/** @} */ /* End of RaceUp_FDCAN_Exported_Variables */

/* Exported functions --------------------------------------------------------*/
/** @defgroup RaceUp_FDCAN_Exported_Functions FDCAN Exported Functions
 * @{
 */

/** @defgroup RaceUp_FDCAN_Exported_Functions_Init Initialization and Control
 * @brief    Initialization, Start, and Filter configuration.
 * @{
 */

/**
 * @brief  Initialize the FDCAN instance with specific baudrate settings.
 * @note   Baudrate = KernelClock / (Prescaler * (1 + TimeSeg1 + TimeSeg2)).
 * This function automatically configures the timing parameters based on the enum.
 * @param  Instance FDCAN Instance (FDCAN1 or FDCAN2).
 * @param  br       Nominal Bitrate selection (@ref RU_FDCAN_NominalBitrate).
 * @param  global_filter Configuration for non-matching frames (e.g., FDCAN_REJECT, FDCAN_ACCEPT_IN_RX_FIFO0).
 */
void RU_FDCAN_Init(FDCAN_GlobalTypeDef *Instance, RU_FDCAN_NominalBitrate br, uint32_t global_filter);

/**
 * @brief  Starts the FDCAN module and enables interrupts.
 * @param  Instance FDCAN Instance (FDCAN1 or FDCAN2).
 */
void RU_FDCAN_Start(FDCAN_GlobalTypeDef *Instance);

/**
 * @brief  Configure a standard ID acceptance filter.
 * @param  Instance FDCAN Instance.
 * @param  type     FDCAN_FILTER_RANGE, FDCAN_FILTER_DUAL, or FDCAN_FILTER_MASK.
 * @param  config   Filter configuration (e.g., FDCAN_FILTER_TO_RXFIFO0).
 * @param  id1      First ID (or Filter ID).
 * @param  id2      Second ID (or Mask).
 */
void RU_FDCAN_AddFilter(FDCAN_GlobalTypeDef *Instance, uint32_t type, uint32_t config, uint16_t id1, uint16_t id2);

/** @} */ /* End of RaceUp_FDCAN_Exported_Functions_Init */


/** @defgroup RaceUp_FDCAN_Exported_Functions_Callback Callback Registration
 * @brief    Functions to register user callbacks.
 * @{
 */

/**
 * @brief  Register a callback function for RX FIFO 0.
 * @param  Instance FDCAN Instance.
 * @param  Callback Function pointer: void func(uint16_t id, uint8_t* data, uint8_t len).
 */
void RU_FDCAN_RegisterRxFIFO0Callback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(uint16_t id, uint8_t* data, uint8_t len));

/**
 * @brief  Register a callback function for RX FIFO 1.
 * @param  Instance FDCAN Instance.
 * @param  Callback Function pointer: void func(uint16_t id, uint8_t* data, uint8_t len).
 */
void RU_FDCAN_RegisterRxFIFO1Callback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(uint16_t id, uint8_t* data, uint8_t len));

/**
 * @brief  Register a callback function for Errors.
 * @param  Instance FDCAN Instance.
 * @param  Callback Function pointer: void func(uint32_t error_flags).
 */
void RU_FDCAN_RegisterErrorCallback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(uint32_t error_flags));

/**
 * @brief  Register a callback function for High Priority Messages.
 * @param  Instance FDCAN Instance.
 * @param  Callback Function pointer: void func(FDCAN_HpMsgStatusTypeDef* hpStatus).
 */
void RU_FDCAN_RegisterHpCallback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(FDCAN_HpMsgStatusTypeDef* hpStatus));

/** @} */ /* End of RaceUp_FDCAN_Exported_Functions_Callback */


/** @defgroup RaceUp_FDCAN_Exported_Functions_IO Input/Output
 * @brief    Transmission and Interrupt handling.
 * @{
 */

/**
 * @brief  Send a CAN frame (Standard ID, Max 8 Bytes).
 * @param  Instance FDCAN Instance.
 * @param  id       11-bit Standard ID.
 * @param  data     Pointer to data buffer.
 * @param  len      Data length (0 to 8).
 */
void RU_FDCAN_Send(FDCAN_GlobalTypeDef *Instance, uint16_t id, uint8_t* data, uint8_t len);

/**
 * @brief  Interrupt Handler Helper.
 * @details Should be called from the FDCANx_IT0_IRQHandler and FDCANx_IT1_IRQHandler.
 * Delegates to HAL_FDCAN_IRQHandler.
 * @param  Instance FDCAN Instance.
 * @see HAL_FDCAN_IRQHandler in stm32h5xx_hal_fdcan.c
 */
void RU_FDCAN_IRQHandler(FDCAN_GlobalTypeDef *Instance);

/** @} */ /* End of RaceUp_FDCAN_Exported_Functions_IO */

/** @} */ /* End of FDCAN_Exported_Functions */

/** @} */ /* End of RaceUp_FDCAN */
/** @} */ /* End of RaceUp_Drivers */

#ifdef __cplusplus
}
#endif

#endif /* _RACEUP_FDCAN_H */
