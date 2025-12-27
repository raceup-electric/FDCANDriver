#include "stm32h563.h"
#include "printf.h"

#include "drivers/gpio.h"
#include "drivers/rcc.h"
#include "drivers/usart.h"
#include "drivers/systick.h"

static volatile uint32_t timems = 0;

void SysTick_Handler() {
  timems++;
}

static USART_TypeDef *DEBUG_UART = USART3;

void _putchar(char c) {
    usart_send_char(DEBUG_UART, c);
}

void SystemInit(void)
{
    systick_init(4000);

    rcc_enable_usart(USART3);
    
    rcc_enable_gpio(BANK('D'));
    
    gpio_setup(PIN('D', 8), GPIO_MODE_AF, GPIO_NOPULL, GPIO_OTYPE_PP, GPIO_SPEED_MEDIUM, 7);
    gpio_setup(PIN('D', 9), GPIO_MODE_AF, GPIO_NOPULL, GPIO_OTYPE_PP, GPIO_SPEED_MEDIUM, 7);

    usart_setup(USART3, 115200, 32000000);
}

int main(void)
{
  uint16_t green_led = PIN('B', 0);
  uint16_t yellow_led = PIN('F', 4);
  uint16_t red_led = PIN('G', 4);
  uint16_t button = PIN('C', 13);

  rcc_enable_gpio(PINBANK(button));
  rcc_enable_gpio(PINBANK(green_led));
  rcc_enable_gpio(PINBANK(yellow_led));
  rcc_enable_gpio(PINBANK(red_led));

  gpio_setup(green_led, GPIO_MODE_OUTPUT, GPIO_NOPULL, GPIO_OTYPE_PP, GPIO_SPEED_LOW, 0);
  gpio_setup(yellow_led, GPIO_MODE_OUTPUT, GPIO_NOPULL, GPIO_OTYPE_PP, GPIO_SPEED_LOW, 0);
  gpio_setup(red_led, GPIO_MODE_OUTPUT, GPIO_NOPULL, GPIO_OTYPE_PP, GPIO_SPEED_LOW, 0);

  gpio_setup(button, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_OTYPE_PP, GPIO_SPEED_LOW, 0);

  gpio_write(yellow_led, true);
  gpio_write(red_led, false);

  uint32_t last_time = timems;

  while (1) {
    gpio_write(green_led, gpio_read(button));

    if ((timems - last_time) > 500) {
      last_time = timems;
      gpio_toggle(yellow_led);
      gpio_toggle(red_led);
      printf("timems: %d\r\n", timems);
    }
  }

  return 0;
}
