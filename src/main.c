#include "stm32h563.h"
#include "printf.h"

// // will go in .bss
// static char uvar __attribute__((used));
//
// // will go in .data
// static char ivar __attribute__((used)) = 5;

static volatile uint32_t timems = 0;

void SysTick_Handler() {
  timems++;
}

void SystemInit(void)
{
  systick_init(4000);
  usart_init();
}

int main(void)
{
  // green: PB0, yellow: PF4, red: PG4
  uint16_t green_led = PIN('B', 0);
  uint16_t yellow_led = PIN('F', 4);
  uint16_t red_led = PIN('G', 4);
  uint16_t button = PIN('C', 13);

  rcc_enable_bank(PINBANK(button));
  rcc_enable_bank(PINBANK(green_led));
  rcc_enable_bank(PINBANK(yellow_led));
  rcc_enable_bank(PINBANK(red_led));

  gpio_set_mode(green_led, GPIO_MODE_OUTPUT);
  gpio_set_mode(yellow_led, GPIO_MODE_OUTPUT);
  gpio_set_mode(red_led, GPIO_MODE_OUTPUT);
  gpio_set_mode(button, GPIO_MODE_INPUT);

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
