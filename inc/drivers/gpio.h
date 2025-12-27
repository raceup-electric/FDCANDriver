#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    GPIO_MODE_INPUT     = 0,
    GPIO_MODE_OUTPUT    = 1,
    GPIO_MODE_AF        = 2,
    GPIO_MODE_ANALOG    = 3
} GPIO_Mode_t;

typedef enum {
    GPIO_NOPULL   = 0,
    GPIO_PULLUP   = 1,
    GPIO_PULLDOWN = 2
} GPIO_Pull_t;

typedef enum {
    GPIO_OTYPE_PP = 0, // Push-Pull
    GPIO_OTYPE_OD = 1  // Open-Drain
} GPIO_Output_t;

typedef enum {
    GPIO_SPEED_LOW       = 0,
    GPIO_SPEED_MEDIUM    = 1,
    GPIO_SPEED_HIGH      = 2,
    GPIO_SPEED_VERY_HIGH = 3
} GPIO_Speed_t;

/**
 * @brief Configure a GPIO pin.
 * @param pin The pin identifier (use PIN('A', 0) macro).
 * @param mode Input, Output, AF, or Analog.
 * @param pull Pull-up, Pull-down, or None.
 * @param otype Output type (Push-Pull or Open-Drain).
 * @param speed Output slew rate.
 * @param alt_func Alternate function number (0-15). Only used if mode == GPIO_Mode_AF.
 */
void gpio_setup(uint16_t pin, GPIO_Mode_t mode, GPIO_Pull_t pull, GPIO_Output_t otype, GPIO_Speed_t speed, uint8_t alt_func);

/**
 * @brief Simple write to pin.
 */
void gpio_write(uint16_t pin, bool val);

/**
 * @brief Toggle pin state.
 */
void gpio_toggle(uint16_t pin);

/**
 * @brief Read pin state.
 */
bool gpio_read(uint16_t pin);

#endif
