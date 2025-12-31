#ifndef FDCAN_H
#define FDCAN_H

#include <stdint.h>
#include <stdbool.h>

#include "stm32h563.h"

typedef enum {
  FDCAN_RX_FIFO0 = 0,
  FDCAN_RX_FIFO1 = 1
} FDCAN_RxFIFO_t;

/**
  * @brief FDCAN Init structure definition
  */
typedef struct
{
  // uint32_t ClockDivider;                 /*!< Specifies the FDCAN kernel clock divider.
                                              // The clock is common to all FDCAN instances.
                                              // This parameter is applied only at initialisation of
                                              // first FDCAN instance.
                                              // This parameter can be a value of @ref FDCAN_clock_divider.   */

  // uint32_t FrameFormat;                  /*!< Specifies the FDCAN frame format.
                                              // This parameter can be a value of @ref FDCAN_frame_format     */

  bool Loopback;                         /*!< Specifies the FDCAN mode.
                                              This parameter can be a value of @ref FDCAN_operating_mode   */

  // FunctionalState AutoRetransmission;    /*!< Enable or disable the automatic retransmission mode.
                                              // This parameter can be set to ENABLE or DISABLE               */

  // FunctionalState TransmitPause;         /*!< Enable or disable the Transmit Pause feature.
                                              // This parameter can be set to ENABLE or DISABLE               */

  // FunctionalState ProtocolException;      /*!< Enable or disable the Protocol Exception Handling.
                                              // This parameter can be set to ENABLE or DISABLE               */

  uint32_t NominalPrescaler;             /*!< Specifies the value by which the oscillator frequency is
                                              divided for generating the nominal bit time quanta.
                                              This parameter must be a number between 1 and 512            */

  uint32_t NominalSyncJumpWidth;         /*!< Specifies the maximum number of time quanta the FDCAN
                                              hardware is allowed to lengthen or shorten a bit to perform
                                              resynchronization.
                                              This parameter must be a number between 1 and 128            */

  uint32_t NominalTimeSeg1;              /*!< Specifies the number of time quanta in Bit Segment 1.
                                              This parameter must be a number between 2 and 256            */

  uint32_t NominalTimeSeg2;              /*!< Specifies the number of time quanta in Bit Segment 2.
                                              This parameter must be a number between 2 and 128            */

  // uint32_t DataPrescaler;                /*!< Specifies the value by which the oscillator frequency is
  //                                             divided for generating the data bit time quanta.
  //                                             This parameter must be a number between 1 and 32             */
  //
  // uint32_t DataSyncJumpWidth;            /*!< Specifies the maximum number of time quanta the FDCAN
  //                                             hardware is allowed to lengthen or shorten a data bit to
  //                                             perform resynchronization.
  //                                             This parameter must be a number between 1 and 16             */
  //
  // uint32_t DataTimeSeg1;                 /*!< Specifies the number of time quanta in Data Bit Segment 1.
  //                                             This parameter must be a number between 1 and 32             */
  //
  // uint32_t DataTimeSeg2;                 /*!< Specifies the number of time quanta in Data Bit Segment 2.
                                              // This parameter must be a number between 1 and 16             */

  uint32_t StdFiltersNbr;                /*!< Specifies the number of standard Message ID filters.
                                              This parameter must be a number between 0 and 28             */

  uint32_t ExtFiltersNbr;                /*!< Specifies the number of extended Message ID filters.
                                              This parameter must be a number between 0 and 8             */

  bool TxQueue;              /*!< Tx FIFO/Queue Mode selection.
                                              This parameter can be a value of @ref FDCAN_txFifoQueue_Mode */

} FDCAN_Init_t;

/**
  * @brief  FDCAN Message RAM blocks
  */
typedef struct
{
  uint32_t StandardFilterSA; /*!< Specifies the Standard Filter List Start Address.
                                  This parameter must be a 32-bit word address      */

  uint32_t ExtendedFilterSA; /*!< Specifies the Extended Filter List Start Address.
                                  This parameter must be a 32-bit word address      */

  uint32_t RxFIFO0SA;        /*!< Specifies the Rx FIFO 0 Start Address.
                                  This parameter must be a 32-bit word address      */

  uint32_t RxFIFO1SA;        /*!< Specifies the Rx FIFO 1 Start Address.
                                  This parameter must be a 32-bit word address      */

  uint32_t TxEventFIFOSA;    /*!< Specifies the Tx Event FIFO Start Address.
                                  This parameter must be a 32-bit word address      */

  uint32_t TxFIFOQSA;        /*!< Specifies the Tx FIFO/Queue Start Address.
                                  This parameter must be a 32-bit word address      */

} FDCAN_MsgRamAddress_t;

/**
  * @brief  FDCAN handle structure definition
  */
typedef struct
{
  FDCAN_t         *Instance;        /*!< Register base address     */

  FDCAN_Init_t           Init;             /*!< FDCAN required parameters */

  FDCAN_MsgRamAddress_t  msgRam;           /*!< FDCAN Message RAM blocks  */

  uint32_t                    LatestTxFifoQRequest; /*!< FDCAN Tx buffer index
                                               of latest Tx FIFO/Queue request */

  // __IO HAL_FDCAN_State_t State;            /*!< FDCAN communication state */

  // HAL_Lock_t             Lock;             /*!< FDCAN locking object      */

  // __IO uint32_t               ErrorCode;        /*!< FDCAN Error code          */

} FDCAN_Handle_t;

/**
  * @brief  FDCAN filter structure definition
  */
typedef struct
{
  // uint32_t IdType;           /*!< Specifies the identifier type.
  //                                 This parameter can be a value of @ref FDCAN_id_type       */

  uint32_t FilterIndex;      /*!< Specifies the filter which will be initialized.
                                  This parameter must be a number between:
                                   - 0 and (SRAMCAN_FLS_NBR-1), if IdType is FDCAN_STANDARD_ID
                                   - 0 and (SRAMCAN_FLE_NBR-1), if IdType is FDCAN_EXTENDED_ID */

  uint32_t FilterType;       /*!< Specifies the filter type.
                                  This parameter can be a value of @ref FDCAN_filter_type.
                                  The value FDCAN_FILTER_RANGE_NO_EIDM is permitted
                                  only when IdType is FDCAN_EXTENDED_ID.                    */

  uint32_t FilterConfig;     /*!< Specifies the filter configuration.
                                  This parameter can be a value of @ref FDCAN_filter_config */

  uint32_t FilterID1;        /*!< Specifies the filter identification 1.
                                  This parameter must be a number between:
                                   - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
                                   - 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID       */

  uint32_t FilterID2;        /*!< Specifies the filter identification 2.
                                  This parameter must be a number between:
                                   - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
                                   - 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID       */

} FDCAN_Filter_t;

/**
  * @brief  FDCAN Tx header structure definition
  */
typedef struct
{
  uint32_t Identifier;          /*!< Specifies the identifier.
                                     This parameter must be a number between:
                                      - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
                                      - 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID               */

  // uint32_t IdType;              /*!< Specifies the identifier type for the message that will be
  //                                    transmitted.
  //                                    This parameter can be a value of @ref FDCAN_id_type               */

  // uint32_t TxFrameType;         /*!< Specifies the frame type of the message that will be transmitted.
  //                                    This parameter can be a value of @ref FDCAN_frame_type            */

  uint32_t DataLength;          /*!< Specifies the length of the frame that will be transmitted.
                                      This parameter can be a value of @ref FDCAN_data_length_code     */

  uint32_t ErrorStateIndicator; /*!< Specifies the error state indicator.
                                     This parameter can be a value of @ref FDCAN_error_state_indicator */

  // uint32_t BitRateSwitch;       /*!< Specifies whether the Tx frame will be transmitted with or without
  //                                    bit rate switching.
  //                                    This parameter can be a value of @ref FDCAN_bit_rate_switching    */

  // uint32_t FDFormat;            /*!< Specifies whether the Tx frame will be transmitted in classic or
  //                                    FD format.
                                     // This parameter can be a value of @ref FDCAN_format                */

  // uint32_t TxEventFifoControl;  /*!< Specifies the event FIFO control.
  //                                    This parameter can be a value of @ref FDCAN_EFC                   */

  // uint32_t MessageMarker;       /*!< Specifies the message marker to be copied into Tx Event FIFO
  //                                    element for identification of Tx message status.
                                     // This parameter must be a number between 0 and 0xFF                */

  uint8_t *Data;

} FDCAN_TxElement_t;

/**
  * @brief  FDCAN Rx header structure definition
  */
typedef struct
{
  uint32_t Identifier;            /*!< Specifies the identifier.
                                       This parameter must be a number between:
                                        - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
                                        - 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID               */

  // uint32_t IdType;                /*!< Specifies the identifier type of the received message.
  //                                      This parameter can be a value of @ref FDCAN_id_type               */
  //
  // uint32_t RxFrameType;           /*!< Specifies the the received message frame type.
  //                                      This parameter can be a value of @ref FDCAN_frame_type            */

  uint32_t DataLength;            /*!< Specifies the received frame length.
                                        This parameter can be a value of @ref FDCAN_data_length_code     */

  uint32_t ErrorStateIndicator;   /*!< Specifies the error state indicator.
                                       This parameter can be a value of @ref FDCAN_error_state_indicator */

  // uint32_t BitRateSwitch;         /*!< Specifies whether the Rx frame is received with or without bit
  //                                      rate switching.
  //                                      This parameter can be a value of @ref FDCAN_bit_rate_switching    */
  //
  // uint32_t FDFormat;              /*!< Specifies whether the Rx frame is received in classic or FD
  //                                      format.
  //                                      This parameter can be a value of @ref FDCAN_format                */

  uint32_t RxTimestamp;           /*!< Specifies the timestamp counter value captured on start of frame
                                       reception.
                                       This parameter must be a number between 0 and 0xFFFF              */

  uint32_t FilterIndex;           /*!< Specifies the index of matching Rx acceptance filter element.
                                       This parameter must be a number between:
                                        - 0 and (SRAMCAN_FLS_NBR-1), if IdType is FDCAN_STANDARD_ID
                                        - 0 and (SRAMCAN_FLE_NBR-1), if IdType is FDCAN_EXTENDED_ID
                                       When the frame is a Non-Filter matching frame, this parameter
                                       is unused.                                                        */

  // uint32_t IsFilterMatchingFrame; /*!< Specifies whether the accepted frame did not match any Rx filter.
  //                                      Acceptance of non-matching frames may be enabled via
  //                                      HAL_FDCAN_ConfigGlobalFilter().
  //                                      This parameter takes 0 if the frame matched an Rx filter or
  //                                      1 if it did not match any Rx filter                               */

  uint8_t *Data;

} FDCAN_RxElement_t;

int fdcan_init(FDCAN_Handle_t *fdcan);
int fdcan_set_filter(FDCAN_Handle_t *fdcan, const FDCAN_Filter_t *filter);
int fdcan_send(FDCAN_Handle_t *fdcan, const FDCAN_TxElement_t *tx);
int fdcan_rxfifo_level(const FDCAN_Handle_t *fdcan, FDCAN_RxFIFO_t fifo);
int fdcan_receive(FDCAN_Handle_t *fdcan, FDCAN_RxElement_t *rx, FDCAN_RxFIFO_t fifo);
int fdcan_is_available(FDCAN_Handle_t *fdcan);

#endif
