#include "drivers/gpio.h"
#include "stm32h563.h"

void gpio_setup(uint16_t pin, GPIO_Mode_t mode, GPIO_Pull_t pull, GPIO_Output_t otype, GPIO_Speed_t speed, uint8_t alt_func) {
    GPIO_t *gpio = GPIO(PINBANK(pin));
    uint8_t pinno = PINNO(pin);
    
    // 1. Configure Mode
    gpio->MODER &= ~(3U << (pinno * 2));
    gpio->MODER |= ((mode & 3U) << (pinno * 2));

    // 2. Configure Pull resistors
    gpio->PUPDR &= ~(3U << (pinno * 2));
    gpio->PUPDR |= ((pull & 3U) << (pinno * 2));

    // 3. Configure Output Type
    if (otype == GPIO_OTYPE_OD) {
        gpio->OTYPER |= (1U << pinno);
    } else {
        gpio->OTYPER &= ~(1U << pinno);
    }

    // 4. Configure Speed
    gpio->OSPEEDR &= ~(3U << (pinno * 2));
    gpio->OSPEEDR |= ((speed & 3U) << (pinno * 2));

    // 5. Configure Alternate Function (only if mode is AF)
    if (mode == GPIO_MODE_AF) {
        if (pinno < 8) {
            gpio->AFR[0] &= ~(0xFU << (pinno * 4));
            gpio->AFR[0] |= ((alt_func & 0xFU) << (pinno * 4));
        } else {
            gpio->AFR[1] &= ~(0xFU << ((pinno - 8) * 4));
            gpio->AFR[1] |= ((alt_func & 0xFU) << ((pinno - 8) * 4));
        }
    }
}

void gpio_write(uint16_t pin, bool val) {
    GPIO_t *gpio = GPIO(PINBANK(pin));
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
    GPIO_t *gpio = GPIO(PINBANK(pin));
    return (bool)((gpio->IDR >> PINNO(pin)) & 1U);
}
