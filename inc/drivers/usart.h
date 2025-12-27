#ifndef USART_H
#define USART_H

#include "stm32h563.h"

/**
 * @brief Initialize a USART peripheral.
 * @param usart Pointer to the USART instance (e.g., USART3).
 * @param baudrate Desired baud rate (e.g., 115200).
 * @param pclk_hz Frequency of the peripheral clock feeding this USART in Hz.
 * NOTE: Ensure you pass the correct clock freq (e.g. 64000000).
 */
void usart_setup(USART_t *usart, uint32_t baudrate, uint32_t pclk_hz);

/**
 * @brief Send a single character.
 * @param usart Pointer to the USART instance.
 * @param c Character to send.
 */
void usart_send_char(USART_t *usart, char c);

/**
 * @brief Send a null-terminated string.
 */
void usart_send_string(USART_t *usart, const char *str);

/**
 * @brief Check if data is available to read.
 */
bool usart_data_available(USART_t *usart);

/**
 * @brief Read a character (blocking).
 */
char usart_receive_char(USART_t *usart);

#endif
