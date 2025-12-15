#include "stm32h563.h"

/* ========================================================================== */
/* GPIO                                    */
/* ========================================================================== */

void gpio_set_mode(uint16_t pin, uint8_t mode) {
    GPIO_TypeDef *gpio = GPIO(PINBANK(pin));
    uint8_t pinno = PINNO(pin);
    
    // Clear existing mode (2 bits per pin)
    gpio->MODER &= ~(3U << (pinno * 2));
    // Set new mode
    gpio->MODER |= (mode & 3U) << (pinno * 2);
}

void gpio_write(uint16_t pin, bool val) {
    GPIO_TypeDef *gpio = GPIO(PINBANK(pin));
    // BSRR: Lower 16 bits set, Upper 16 bits reset
    if (val) {
        gpio->BSRR = (1U << PINNO(pin));
    } else {
        gpio->BSRR = (1U << (PINNO(pin) + 16));
    }
}

void gpio_toggle(uint16_t pin) {
    gpio_write(pin, !gpio_read(pin));
}

bool gpio_read(uint16_t pin) {
    GPIO_TypeDef *gpio = GPIO(PINBANK(pin));
    return (bool)((gpio->IDR >> PINNO(pin)) & 1U);
}

/* ========================================================================== */
/* RCC                                    */
/* ========================================================================== */

void rcc_enable_bank(uint8_t bank) {
    // GPIO banks A-K are on AHB2.
    // Bank 0 (A) -> Bit 0, Bank 1 (B) -> Bit 1, etc.
    RCC->AHB2ENR |= BIT(bank);
}

/* ========================================================================== */
/* SYSTICK                                  */
/* ========================================================================== */

void systick_init(uint32_t ticks) {
    if ((ticks - 1) > 0xFFFFFF) return; // Max value is 24 bits
    
    SysTick->LOAD = ticks - 1;
    SysTick->VAL = 0;
    
    // Enable SysTick | Enable Interrupt | Source = External Clock
    SysTick->CTRL = BIT(0) | BIT(1) | ~BIT(2); 
}

/* ========================================================================== */
/* USART                                   */
/* ========================================================================== */

void usart_init(void) {
    // 1. Enable USART3 Clock (APB1)
    RCC->APB1LENR |= BIT(18); 
    
    // 2. Configure Pins PD8 (TX) and PD9 (RX)
    uint16_t pD8 = PIN('D', 8);
    uint16_t pD9 = PIN('D', 9);
    
    rcc_enable_bank(BANK('D'));
    
    gpio_set_mode(pD8, GPIO_MODE_AF);
    gpio_set_mode(pD9, GPIO_MODE_AF);
    
    // 3. Set Alternate Function (AF7) for PD8/PD9
    // AF7 is commonly USART3 on H5 series (Verify in datasheet)
    // AFR[1] controls pins 8-15. Pin 8 is index 0 in AFR[1], Pin 9 is index 1.
    // 4 bits per pin.
    
    // Clear AF for Pin 8 and Pin 9
    GPIO(BANK('D'))->AFR[1] &= ~(0xFFUL); 
    // Set AF7 (0x7) for both
    GPIO(BANK('D'))->AFR[1] |= (0x77UL);

    // 4. Configure UART
    USART3->CR1 = 0; // Disable first
    
    // Baud Rate Configuration
    // Note: This assumes a specific peripheral clock frequency.
    // 0x0D05 = 3333 decimal. 
    USART3->BRR = 0x0D05;
    
    // Enable UART (UE), TE (Transmitter), RE (Receiver)
    USART3->CR1 |= BIT(0) | BIT(3) | BIT(2); 
}

void _putchar(char c) {
    // Wait for Transmit Data Register Empty (TXE bit 7)
    while (!(USART3->ISR & BIT(7)));
    USART3->TDR = c;
}
