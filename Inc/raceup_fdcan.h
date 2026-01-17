/*
 * File: raceup_fdcan.h
 * Description: Simplified FDCAN wrapper for STM32H5 (Supports FDCAN1 & FDCAN2)
 * Constraints: Standard ID only, No BRS, Max 8 Bytes payload.
 */

#ifndef RACEUP_FDCAN_H
#define RACEUP_FDCAN_H

#include "stm32h5xx_hal.h"

#define RACEUP_CAN_DLC_MAX      8

typedef struct {
    FDCAN_HandleTypeDef hfdcan;
    void (*RxFIFO0Callback)(uint16_t id, uint8_t* data, uint8_t len);
    void (*RxFIFO1Callback)(uint16_t id, uint8_t* data, uint8_t len);
    void (*RxPriorityCallback)(uint16_t id, uint8_t* data, uint8_t len);
    void (*ErrorCallback)(uint32_t error_flags);
    void (*HpCallback)(FDCAN_HpMsgStatusTypeDef* hpstatus);
    uint8_t Initialized;
} RU_FDCAN_HandleTypeDef;

typedef enum {
  FDCAN_NOMINAL_BR_125,
  FDCAN_NOMINAL_BR_250,
  FDCAN_NOMINAL_BR_500,
  FDCAN_NOMINAL_BR_1000
} RU_FDCAN_NominalBitrate;

/* Global Wrapper Instances (Accessible for MSP/IT) */
extern RU_FDCAN_HandleTypeDef hRUCAN1;
#ifdef FDCAN2
extern RU_FDCAN_HandleTypeDef hRUCAN2;
#endif

/* --- Public API --- */

/**
 * @brief Initialize the FDCAN instance with specific baudrate settings.
 * @param Instance: FDCAN1 or FDCAN2
 * @param Prescaler: Prescaler for the kernel clock (e.g., 1 for 20MHz)
 * @param TimeSeg1: Time Segment 1
 * @param TimeSeg2: Time Segment 2
 * @param SyncJumpWidth: Sync Jump Width
 * @note  Baudrate = KernelClock / (Prescaler * (1 + TimeSeg1 + TimeSeg2))
 */
void RU_FDCAN_Init(FDCAN_GlobalTypeDef *Instance, RU_FDCAN_NominalBitrate br, uint32_t global_filter);

void RU_FDCAN_Start(FDCAN_GlobalTypeDef *Instance);

/**
 * @brief Register a callback function to be called when a message is received.
 */
void RU_FDCAN_RegisterRxFIFO0Callback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(uint16_t id, uint8_t* data, uint8_t len));
void RU_FDCAN_RegisterRxFIFO1Callback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(uint16_t id, uint8_t* data, uint8_t len));
void RU_FDCAN_RegisterErrorCallback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(uint32_t error_flags));
void RU_FDCAN_RegisterHpCallback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(FDCAN_HpMsgStatusTypeDef* hpStatus));

/**
 * @brief Configure a standard ID acceptance filter.
 * @param id: The 11-bit Standard ID to match.
 * @param mask: The mask (0x7FF for exact match).
 */
void RU_FDCAN_AddFilter(FDCAN_GlobalTypeDef *Instance, uint32_t type, uint32_t config, uint16_t id1, uint16_t id2);

/**
 * @brief Send a CAN frame (Standard ID, Max 8 Bytes).
 * @param id: 11-bit Standard ID.
 * @param data: Pointer to data buffer.
 * @param len: Data length (0 to 8).
 */
void RU_FDCAN_Send(FDCAN_GlobalTypeDef *Instance, uint16_t id, uint8_t* data, uint8_t len);

/**
 * @brief Interrupt Handler Helper.
 */
void RU_FDCAN_IRQHandler(FDCAN_GlobalTypeDef *Instance);

#endif /* RACEUP_FDCAN_H */
