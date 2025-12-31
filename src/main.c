#include "stm32h563.h"
#include "printf.h"

#include "drivers/fdcan.h"
#include "drivers/gpio.h"
#include "drivers/rcc.h"
#include "drivers/usart.h"
#include "drivers/systick.h"

static volatile uint32_t timems = 0;

void SysTick_Handler() {
  timems++;
}

static USART_t *DEBUG_UART = USART3;
FDCAN_Handle_t hfdcan1;

void _putchar(char c) {
    usart_send_char(DEBUG_UART, c);
}

void SystemInit(void)
{
    systick_init(4000);

    rcc_enable_usart(USART3);

    rcc_enable_gpio(BANK('D'));
    
    gpio_setup(PIN('D', 8), GPIO_MODE_AF, GPIO_PULLUP, GPIO_OTYPE_PP, GPIO_SPEED_MEDIUM, 7);
    gpio_setup(PIN('D', 9), GPIO_MODE_AF, GPIO_PULLUP, GPIO_OTYPE_PP, GPIO_SPEED_MEDIUM, 7);

    usart_setup(USART3, 115200, 32000000);
}

void FDCAN_Config(void) {
    // FDCAN1 Initialization on PA11/PA12
    hfdcan1.Instance = FDCAN1;
    
    // Clock: 40MHz. Target: 500kbps. Total TQ: 80.
    hfdcan1.Init.NominalPrescaler = 1;
    hfdcan1.Init.NominalTimeSeg1 = 63; 
    hfdcan1.Init.NominalTimeSeg2 = 16;
    hfdcan1.Init.NominalSyncJumpWidth = 4; // SJW
    
    // Setup Filters
    hfdcan1.Init.StdFiltersNbr = 1;
    hfdcan1.Init.ExtFiltersNbr = 0;
    
    // Note: The driver provided sets FDOE and BRSE bits unconditionally.
    // This config allows FD frames. Since Nominal and Data timings 
    // are derived from the same Init variables in your driver,
    // both phases will run at 500kbps.
    hfdcan1.Init.Loopback = true; // Set to true to test without external transceiver
    hfdcan1.Init.TxQueue = true;

    if (fdcan_init(&hfdcan1) != 0) {
        printf("FDCAN Init Failed!\r\n");
        while(1);
    }

    // Configure a "Accept All" Filter
    // Filter Index 0, Type=Range, Config=Store in FIFO0, ID1=0, ID2=0x7FF (All Std IDs)
    FDCAN_Filter_t filter;
    filter.FilterIndex = 0;
    filter.FilterType = 1;   // Classic Filter: Mask
    filter.FilterConfig = 1; // Store in FIFO0
    filter.FilterID1 = 0x000; // Value
    filter.FilterID2 = 0x000; // Mask (0x000 means accept all)
    
    fdcan_set_filter(&hfdcan1, &filter);
    
    printf("FDCAN Initialized at 500kbps\r\n");
}


int main(void)
{
  printf("starting...\r\n");
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

  gpio_setup(button, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_OTYPE_PP, GPIO_SPEED_LOW, 0);

  gpio_write(yellow_led, true);
  gpio_write(red_led, false);

  printf("configuring FDCAN...\r\n");
  // FDCAN_Config();

  uint32_t last_time = timems;

  // Tx Header
  FDCAN_TxElement_t txHeader;
  uint8_t txData[8] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80};
  
  txHeader.Identifier = 0x123;
  txHeader.DataLength = 8; // DLC 8 bytes
  txHeader.Data = txData;
  // Note: Your driver 'fdcan_send' ignores ID Type (Std/Ext) configuration 
  // and assumes Standard ID by shifting << 18. Ensure you use Standard IDs.

  // Rx Header
  FDCAN_RxElement_t rxHeader;
  uint8_t rxData[64];
  rxHeader.Data = rxData;


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
