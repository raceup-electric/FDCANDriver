#include "drivers/systick.h"

void systick_init(uint32_t ticks) {
    if ((ticks - 1) > 0xFFFFFF) return; // Max value is 24 bits
    
    SysTick->LOAD = ticks - 1;
    SysTick->VAL = 0;
    
    // Enable SysTick | Enable Interrupt | Source = External Clock
    // CTRL: Bit 0=ENABLE, Bit 1=TICKINT, Bit 2=CLKSOURCE
    // Setting CLKSOURCE to 0 (External) is effectively HSI/8 or similar on some STs, 
    // or Processor Clock if set to 1.
    // The previous implementation used ~BIT(2), implying External Clock (AHB/8).
    SysTick->CTRL = BIT(0) | BIT(1) | ~BIT(2); 
}
