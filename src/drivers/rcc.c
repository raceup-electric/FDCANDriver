#include "drivers/rcc.h"

void rcc_enable_gpio(uint8_t bank_idx) {
    RCC->AHB2ENR |= BIT(bank_idx);
}

void rcc_enable_usart(USART_t *usart) {
    if (usart == USART1) {
        RCC->APB2ENR |= BIT(14); // USART1
    } else if (usart == USART2) {
        RCC->APB1LENR |= BIT(17); // USART2
    } else if (usart == USART3) {
        RCC->APB1LENR |= BIT(18); // USART3
    }
}

void rcc_enable_fdcan() {
  RCC->APB1HENR |= BIT(9);
}

void pll1_q_init() {
  RCC->CR |= BIT(16);
  while(!(RCC->CR & BIT(17))); // wait for HSE to be ready
  
  if (RCC->CR & BIT(24)) {
    RCC->CR &= ~BIT(24); // if PLL1 is enabled, turn it off first
    while (RCC->CR & BIT(25));
  }

  RCC->PLL1CFGR &= ~(0b11);
  RCC->PLL1CFGR |= 0b10; // PLL1SCR 10=HSE
  
  RCC->PLL1CFGR &= ~(0b111111 << 8);
  RCC->PLL1DIVR &= ~(0b111111111 | (0b1111111 << 16));

  // set M = 2
  RCC->PLL1CFGR |= (0b10 << 8);

  // set N = 40
  RCC->PLL1DIVR |= (39);

  // set Q = 4
  RCC->PLL1DIVR |= (0b11 << 16);

  RCC->PLL1CFGR |= BIT(17); // enable PLL1Q
  
  RCC->CR |= BIT(24);
  while (!(RCC->CR & BIT(25)));

  RCC->CCIPR5 &= ~(0b11 << 8);
  RCC->CCIPR5 |= (0b01 << 8);
}

