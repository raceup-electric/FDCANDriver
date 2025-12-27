#include "drivers/usart.h"

void usart_setup(USART_t *usart, uint32_t baudrate, uint32_t pclk_hz) {
    // 1. Disable USART
    usart->CR1 = 0;

    // 2. Configure Baud Rate
    // USARTDIV = pclk / baudrate
    // Note: This simple calculation assumes OVER8=0 (Oversampling by 16)
    // For more precision or OVER8=1, more complex logic is needed.
    uint32_t usart_div = pclk_hz / baudrate;
    usart->BRR = usart_div;

    // 3. Enable Transmitter, Receiver, and UE (UART Enable)
    // TE=Bit 3, RE=Bit 2, UE=Bit 0
    usart->CR1 |= (BIT(3) | BIT(2) | BIT(0));
}

void usart_send_char(USART_t *usart, char c) {
    // Wait for TXE (Transmit Data Register Empty) - Bit 7 in ISR
    while (!(usart->ISR & BIT(7)));
    usart->TDR = c;
}

void usart_send_string(USART_t *usart, const char *str) {
    while (*str) {
        usart_send_char(usart, *str++);
    }
}

bool usart_data_available(USART_t *usart) {
    // Check RXNE (Read Data Register Not Empty) - Bit 5 in ISR
    return (usart->ISR & BIT(5));
}

char usart_receive_char(USART_t *usart) {
    while (!usart_data_available(usart));
    return (char)(usart->RDR);
}
