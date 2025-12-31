#include <stdbool.h>
#include <stddef.h>
#include "fdcan.h"
#include "rcc.h"
#include "gpio.h"
#include "stm32h563.h"

#define FDCAN_ELEMENT_MASK_STDID ((uint32_t)0x1FFC0000U) /* Standard Identifier         */
#define FDCAN_ELEMENT_MASK_EXTID ((uint32_t)0x1FFFFFFFU) /* Extended Identifier         */
#define FDCAN_ELEMENT_MASK_RTR   ((uint32_t)0x20000000U) /* Remote Transmission Request */
#define FDCAN_ELEMENT_MASK_XTD   ((uint32_t)0x40000000U) /* Extended Identifier         */
#define FDCAN_ELEMENT_MASK_ESI   ((uint32_t)0x80000000U) /* Error State Indicator       */
#define FDCAN_ELEMENT_MASK_TS    ((uint32_t)0x0000FFFFU) /* Timestamp                   */
#define FDCAN_ELEMENT_MASK_DLC   ((uint32_t)0x000F0000U) /* Data Length Code            */
#define FDCAN_ELEMENT_MASK_BRS   ((uint32_t)0x00100000U) /* Bit Rate Switch             */
#define FDCAN_ELEMENT_MASK_FDF   ((uint32_t)0x00200000U) /* FD Format                   */
#define FDCAN_ELEMENT_MASK_EFC   ((uint32_t)0x00800000U) /* Event FIFO Control          */
#define FDCAN_ELEMENT_MASK_MM    ((uint32_t)0xFF000000U) /* Message Marker              */
#define FDCAN_ELEMENT_MASK_FIDX  ((uint32_t)0x7F000000U) /* Filter Index                */
#define FDCAN_ELEMENT_MASK_ANMF  ((uint32_t)0x80000000U) /* Accepted Non-matching Frame */
#define FDCAN_ELEMENT_MASK_ET    ((uint32_t)0x00C00000U) /* Event type                  */

#define SRAMCAN_FLS_NBR                  (28U)         /* Max. Filter List Standard Number      */
#define SRAMCAN_FLE_NBR                  ( 8U)         /* Max. Filter List Extended Number      */
#define SRAMCAN_RF0_NBR                  ( 3U)         /* RX FIFO 0 Elements Number             */
#define SRAMCAN_RF1_NBR                  ( 3U)         /* RX FIFO 1 Elements Number             */
#define SRAMCAN_TEF_NBR                  ( 3U)         /* TX Event FIFO Elements Number         */
#define SRAMCAN_TFQ_NBR                  ( 3U)         /* TX FIFO/Queue Elements Number         */

#define SRAMCAN_FLS_SIZE            ( 1U * 4U)         /* Filter Standard Element Size in bytes */
#define SRAMCAN_FLE_SIZE            ( 2U * 4U)         /* Filter Extended Element Size in bytes */
#define SRAMCAN_RF0_SIZE            (18U * 4U)         /* RX FIFO 0 Elements Size in bytes      */
#define SRAMCAN_RF1_SIZE            (18U * 4U)         /* RX FIFO 1 Elements Size in bytes      */
#define SRAMCAN_TEF_SIZE            ( 2U * 4U)         /* TX Event FIFO Elements Size in bytes  */
#define SRAMCAN_TFQ_SIZE            (18U * 4U)         /* TX FIFO/Queue Elements Size in bytes  */

#define SRAMCAN_FLSSA ((uint32_t)0)                                                      /* Filter List Standard Start
                                                                                            Address                  */
#define SRAMCAN_FLESA ((uint32_t)(SRAMCAN_FLSSA + (SRAMCAN_FLS_NBR * SRAMCAN_FLS_SIZE))) /* Filter List Extended Start
                                                                                            Address                  */
#define SRAMCAN_RF0SA ((uint32_t)(SRAMCAN_FLESA + (SRAMCAN_FLE_NBR * SRAMCAN_FLE_SIZE))) /* Rx FIFO 0 Start Address  */
#define SRAMCAN_RF1SA ((uint32_t)(SRAMCAN_RF0SA + (SRAMCAN_RF0_NBR * SRAMCAN_RF0_SIZE))) /* Rx FIFO 1 Start Address  */
#define SRAMCAN_TEFSA ((uint32_t)(SRAMCAN_RF1SA + (SRAMCAN_RF1_NBR * SRAMCAN_RF1_SIZE))) /* Tx Event FIFO Start
                                                                                            Address */
#define SRAMCAN_TFQSA ((uint32_t)(SRAMCAN_TEFSA + (SRAMCAN_TEF_NBR * SRAMCAN_TEF_SIZE))) /* Tx FIFO/Queue Start
                                                                                            Address                  */
#define SRAMCAN_SIZE  ((uint32_t)(SRAMCAN_TFQSA + (SRAMCAN_TFQ_NBR * SRAMCAN_TFQ_SIZE))) /* Message RAM size         */

static void _fdcan_init_ram(FDCAN_Handle_t *fdcan)
{
  uint32_t RAMcounter;
  uint32_t SramCanInstanceBase = FDCAN_SRAM_BASE;

  if (fdcan->Instance == FDCAN2)
  {
    SramCanInstanceBase += SRAMCAN_SIZE;
  }

  /* Standard filter list start address */
  fdcan->msgRam.StandardFilterSA = SramCanInstanceBase + SRAMCAN_FLSSA;

  /* Standard filter elements number */
  // MODIFY_REG(hfdcan->Instance->RXGFC, FDCAN_RXGFC_LSS, (hfdcan->Init.StdFiltersNbr << FDCAN_RXGFC_LSS_Pos));
  fdcan->Instance->RXGFC |= (fdcan->Init.StdFiltersNbr << 16);

  /* Extended filter list start address */
  fdcan->msgRam.ExtendedFilterSA = SramCanInstanceBase + SRAMCAN_FLESA;

  /* Extended filter elements number */
  // MODIFY_REG(hfdcan->Instance->RXGFC, FDCAN_RXGFC_LSE, (hfdcan->Init.ExtFiltersNbr << FDCAN_RXGFC_LSE_Pos));

  /* Rx FIFO 0 start address */
  fdcan->msgRam.RxFIFO0SA = SramCanInstanceBase + SRAMCAN_RF0SA;

  /* Rx FIFO 1 start address */
  fdcan->msgRam.RxFIFO1SA = SramCanInstanceBase + SRAMCAN_RF1SA;

  /* Tx event FIFO start address */
  fdcan->msgRam.TxEventFIFOSA = SramCanInstanceBase + SRAMCAN_TEFSA;

  /* Tx FIFO/queue start address */
  fdcan->msgRam.TxFIFOQSA = SramCanInstanceBase + SRAMCAN_TFQSA;

  /* Flush the allocated Message RAM area */
  for (RAMcounter = SramCanInstanceBase; RAMcounter < (SramCanInstanceBase + SRAMCAN_SIZE); RAMcounter += 4U)
  {
    *(uint32_t *)(RAMcounter) = 0x00000000U;
  }
}

static void _fdcan_bittiming(FDCAN_Handle_t *fdcan) {
  // TODO: avoid unneccessary memory allocation
  uint32_t prescaler = (fdcan->Init.NominalPrescaler) - 1;
  uint32_t tseg1 = (fdcan->Init.NominalTimeSeg1) - 1;
  uint32_t tseg2 = (fdcan->Init.NominalTimeSeg2) - 1;
  uint32_t sjw = (fdcan->Init.NominalSyncJumpWidth) - 1;

  fdcan->Instance->NBTP = (tseg2 << 0) |
                (tseg1 << 8) |
                (prescaler << 16) |
                (sjw << 25);

  fdcan->Instance->DBTP = (tseg2 << 0) |
                (tseg1 << 8) |
                (prescaler << 16) |
                (sjw << 25);
}

static void _fdcan_copy2ram(const FDCAN_Handle_t *fdcan, const FDCAN_TxElement_t *tx, uint32_t index) {
  uint32_t tx_element1 = 0, tx_element2 = 0, *tx_address, byte_count;

  tx_element1 = (tx->Identifier << 18U);

  /* Build second word of Tx header element */
  tx_element2 = (tx->DataLength << 16U);
                 
  /* Calculate Tx element address */
  tx_address = (uint32_t *)(fdcan->msgRam.TxFIFOQSA + (index * SRAMCAN_TFQ_SIZE));

  /* Write Tx element header to the message RAM */
  *tx_address = tx_element1;
  tx_address++;
  *tx_address = tx_element2;
  tx_address++;

  /* Write Tx payload to the message RAM */
  for (byte_count = 0; byte_count < tx->DataLength; byte_count += 4U)
  {
    *tx_address = (((uint32_t)tx->Data[byte_count + 3U] << 24U) |
                  ((uint32_t)tx->Data[byte_count + 2U] << 16U) |
                  ((uint32_t)tx->Data[byte_count + 1U] << 8U)  |
                  (uint32_t)tx->Data[byte_count]);
    tx_address++;
  }
}

int fdcan_init(FDCAN_Handle_t *fdcan) {
  if (fdcan == NULL) return 1;

  pll1_q_init();
  rcc_enable_fdcan();

  if (fdcan->Instance == FDCAN1) {
    // A11 -> RX    A12 -> TX
    rcc_enable_gpio(BANK('A'));
    gpio_setup(PIN('A', 11), GPIO_MODE_AF, GPIO_PULLUP, GPIO_OTYPE_PP, GPIO_SPEED_LOW, 9);
    gpio_setup(PIN('A', 12), GPIO_MODE_AF, GPIO_PULLUP, GPIO_OTYPE_PP, GPIO_SPEED_LOW, 9);
  } else if (fdcan->Instance == FDCAN2) {
    // B5 -> RX    B6 -> TX
    rcc_enable_gpio('B');
    gpio_setup(PIN('B', 5), GPIO_MODE_AF, GPIO_PULLUP, GPIO_OTYPE_PP, GPIO_SPEED_LOW, 9);
    gpio_setup(PIN('B', 6), GPIO_MODE_AF, GPIO_PULLUP, GPIO_OTYPE_PP, GPIO_SPEED_LOW, 9);
  }

  fdcan->Instance->CCCR |= BIT(0); // set INIT
  while ((fdcan->Instance->CCCR & BIT(0)) == 0){
    // TODO: implement timeout
  }
  fdcan->Instance->CCCR |= BIT(1); // set CCE

  _fdcan_bittiming(fdcan);

  if (fdcan->Init.Loopback) {
    fdcan->Instance->TEST |= BIT(4);
  }
  
  // fdcan->Instance->CCCR |= (BIT(8) | BIT(9)); // disable FDCAN, use standard CAN 2.0
  
  if(fdcan->Init.TxQueue) {
    fdcan->Instance->TXBC |= BIT(24);
  }

  _fdcan_init_ram(fdcan);

  fdcan->LatestTxFifoQRequest = 0;

  fdcan->Instance->CCCR &= ~BIT(0); // unset INIT
  while ((fdcan->Instance->CCCR & BIT(0)) == 1) {
    // TODO: implement timeout
  }

  return 0;
}

int fdcan_set_filter(FDCAN_Handle_t *fdcan, const FDCAN_Filter_t *filter) {
  uint32_t filter_word, *filter_addr;

  filter_word = ((filter->FilterType << 30U)  |
                (filter->FilterConfig << 27U) |
                (filter->FilterID1 << 16U)    |
                filter->FilterID2);

  filter_addr = (uint32_t *)(fdcan->msgRam.StandardFilterSA + (filter->FilterIndex * SRAMCAN_FLS_SIZE));

  *filter_addr = filter_word;

  return 0;
}

int fdcan_send(FDCAN_Handle_t *fdcan, const FDCAN_TxElement_t *tx) {
  uint32_t index;

  if ((fdcan->Instance->TXFQS & BIT(21)) != 0) {
    return 1;
  } else {
    index = ((fdcan->Instance->TXFQS & (0b11 << 16)) >> 16);
    _fdcan_copy2ram(fdcan, tx, index);
    fdcan->Instance->TXBAR = ((uint32_t)1 << index);
    fdcan->LatestTxFifoQRequest = ((uint32_t)1 << index);
  }

  return 0;
}

int fdcan_rxfifo_level(const FDCAN_Handle_t *fdcan, FDCAN_RxFIFO_t fifo){
  uint32_t fill_level;

  if (fifo == FDCAN_RX_FIFO0) {
    fill_level = (fdcan->Instance->RXF0S & 0b1111);
  } else {
    fill_level = (fdcan->Instance->RXF1S & 0b1111);
  }

  return fill_level;
}

int fdcan_receive(FDCAN_Handle_t *fdcan, FDCAN_RxElement_t *rx, FDCAN_RxFIFO_t fifo) {
  uint32_t *rx_addr, byte_counter, index = 0;
  uint8_t *data;

  if (fifo == FDCAN_RX_FIFO0) {
    if ((fdcan->Instance->RXF0S & 0b1111) != 0) {
      index += ((fdcan->Instance->RXF0S & (0b11 << 8)) >> 8);
      rx_addr = (uint32_t *)(fdcan->msgRam.RxFIFO0SA + (index * SRAMCAN_RF0_SIZE));
    }
  } else {
    if ((fdcan->Instance->RXF1S & 0b1111) != 0) {
      index += ((fdcan->Instance->RXF1S & (0b11 << 8)) >> 8);
      rx_addr = (uint32_t *)(fdcan->msgRam.RxFIFO1SA + (index * SRAMCAN_RF0_SIZE));
    }
  }

  rx->Identifier = ((*rx_addr & FDCAN_ELEMENT_MASK_STDID) >> 18U);
  rx->ErrorStateIndicator = (*rx_addr & FDCAN_ELEMENT_MASK_ESI);

  rx_addr++;

  rx->RxTimestamp = (*rx_addr & FDCAN_ELEMENT_MASK_TS);
  rx->DataLength = ((*rx_addr & FDCAN_ELEMENT_MASK_DLC) >> 16U);
  rx->FilterIndex = ((*rx_addr & FDCAN_ELEMENT_MASK_FIDX) >> 24U);

  rx_addr++;

  data = (uint8_t *)rx_addr;
  for (byte_counter = 0; byte_counter < rx->DataLength; byte_counter++) {
    rx->Data[byte_counter] = data[byte_counter];
  }

  if (fifo == FDCAN_RX_FIFO0) {
    fdcan->Instance->RXF0A = index;
  } else {
    fdcan->Instance->RXF1A = index;
  }

  return 0;
}
// int fdcan_is_available(FDCAN_Handle_t *fdcan);
