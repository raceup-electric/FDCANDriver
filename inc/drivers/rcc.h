#ifndef RCC_H
#define RCC_H

#include <stdint.h>
#include "stm32h563.h"

/**
 * @brief Enable clock for a specific GPIO Bank.
 * @param bank_idx 0 for Bank A, 1 for Bank B, etc.
 */
void rcc_enable_gpio(uint8_t bank_idx);

/**
 * @brief Enable clock for a specific USART Peripheral.
 * @param usart Pointer to the USART instance (USART1, USART3, etc.)
 */
void rcc_enable_usart(USART_t *usart);

/**
 * @brief Enable clock for both FDCAN peripherals.
 */
void rcc_enable_fdcan();

void pll1_q_init();

#endif
