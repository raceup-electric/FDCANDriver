#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
#include "stm32h563.h"

/**
 * @brief Initialize SysTick timer.
 * @param ticks Number of ticks between interrupts.
 */
void systick_init(uint32_t ticks);

#endif
