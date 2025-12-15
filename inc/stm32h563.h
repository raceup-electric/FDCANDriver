#ifndef STM32H563_H
#define STM32H563_H

#include <stdint.h>
#include <stdbool.h>

/* ========================================================================== */
/* MACROS                                   */
/* ========================================================================== */
#define BIT(x)              (1UL << (x))
#define PIN(bank, num)      ((((bank) - 'A') << 8) | (num))
#define BANK(bank)          ((bank) - 'A')
#define PINNO(pin)          ((pin) & 0x00FF)
#define PINBANK(pin)        ((pin) >> 8)

/* ========================================================================== */
/* REGISTER DEFINITIONS                             */
/* ========================================================================== */

/* --- GPIO Registers --- */
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LOCKR;
    volatile uint32_t AFR[2];
    volatile uint32_t BRR;
    volatile uint32_t HSLVR;
    volatile uint32_t SECCFGR;
} GPIO_TypeDef;

/* --- RCC Registers --- */
typedef struct {
    volatile uint32_t CR;
    uint32_t          RESERVED0[3];
    volatile uint32_t HSICFGR;
    volatile uint32_t CRRCR;
    volatile uint32_t CSICFGR;
    volatile uint32_t CFGR1;
    volatile uint32_t CFGR2;
    uint32_t          RESERVED2;
    volatile uint32_t PLL1CFGR;
    volatile uint32_t PLL2CFGR;
    volatile uint32_t PLL3CFGR;
    volatile uint32_t PLL1DIVR;
    volatile uint32_t PLL1FRACR;
    volatile uint32_t PLL2DIVR;
    volatile uint32_t PLL2FRACR;
    volatile uint32_t PLL3DIVR;
    volatile uint32_t PLL3FRACR;
    uint32_t          RESERVED5;
    volatile uint32_t CIER;
    volatile uint32_t CIFR;
    volatile uint32_t CICR;
    uint32_t          RESERVED6;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    uint32_t          RESERVED7;
    volatile uint32_t AHB4RSTR;
    uint32_t          RESERVED9;
    volatile uint32_t APB1LRSTR;
    volatile uint32_t APB1HRSTR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB3RSTR;
    uint32_t          RESERVED10;
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    uint32_t          RESERVED11;
    volatile uint32_t AHB4ENR;
    uint32_t          RESERVED13;
    volatile uint32_t APB1LENR;
    volatile uint32_t APB1HENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB3ENR;
    uint32_t          RESERVED14;
    volatile uint32_t AHB1LPENR;
    volatile uint32_t AHB2LPENR;
    uint32_t          RESERVED15;
    volatile uint32_t AHB4LPENR;
    uint32_t          RESERVED17;
    volatile uint32_t APB1LLPENR;
    volatile uint32_t APB1HLPENR;
    volatile uint32_t APB2LPENR;
    volatile uint32_t APB3LPENR;
    uint32_t          RESERVED18;
    volatile uint32_t CCIPR1;
    volatile uint32_t CCIPR2;
    volatile uint32_t CCIPR3;
    volatile uint32_t CCIPR4;
    volatile uint32_t CCIPR5;
    uint32_t          RESERVED19;
    volatile uint32_t BDCR;
    volatile uint32_t RSR;
} RCC_TypeDef;

/* --- USART Registers --- */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t BRR;
    volatile uint32_t GTPR;
    volatile uint32_t RTOR;
    volatile uint32_t RQR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t RDR;
    volatile uint32_t TDR;
    volatile uint32_t PRESC;
} USART_TypeDef;

/* --- SysTick Registers --- */
typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;

/* --- FDCAN Registers (Added for your Project) --- */
typedef struct {
    volatile uint32_t CREL;   // Core Release Register
    volatile uint32_t ENDN;   // Endian Register
    volatile uint32_t CUST;   // Customer Register
    volatile uint32_t DBTP;   // Data Bit Timing & Prescaler
    volatile uint32_t TEST;   // Test Register
    volatile uint32_t RWD;    // RAM Watchdog
    volatile uint32_t CCCR;   // CC Control Register
    volatile uint32_t NBTP;   // Nominal Bit Timing & Prescaler
    volatile uint32_t TSCC;   // Timestamp Counter Configuration
    volatile uint32_t TSCV;   // Timestamp Counter Value
    volatile uint32_t TOCC;   // Timeout Counter Configuration
    volatile uint32_t TOCV;   // Timeout Counter Value
    volatile uint32_t ECR;    // Error Counter Register
    volatile uint32_t PSR;    // Protocol Status Register
    volatile uint32_t TDCR;   // Transmitter Delay Compensation
    volatile uint32_t IR;     // Interrupt Register
    volatile uint32_t IE;     // Interrupt Enable
    volatile uint32_t ILS;    // Interrupt Line Select
    volatile uint32_t ILE;    // Interrupt Line Enable
    volatile uint32_t RXGFC;  // Global Filter Configuration
    volatile uint32_t XIDAM;  // Extended ID AND Mask
    volatile uint32_t HPMS;   // High Priority Message Status
    volatile uint32_t NDAT1;  // New Data 1
    volatile uint32_t NDAT2;  // New Data 2
    volatile uint32_t RXF0C;  // Rx FIFO 0 Configuration
    volatile uint32_t RXF0S;  // Rx FIFO 0 Status
    volatile uint32_t RXF0A;  // Rx FIFO 0 Acknowledge
    volatile uint32_t RXBC;   // Rx Buffer Configuration
    volatile uint32_t RXF1C;  // Rx FIFO 1 Configuration
    volatile uint32_t RXF1S;  // Rx FIFO 1 Status
    volatile uint32_t RXF1A;  // Rx FIFO 1 Acknowledge
    volatile uint32_t RXESC;  // Rx Buffer Element Size Config
    volatile uint32_t TXBC;   // Tx Buffer Configuration
    volatile uint32_t TXFQS;  // Tx FIFO/Queue Status
    volatile uint32_t TXESC;  // Tx Buffer Element Size Config
    volatile uint32_t TXBAR;  // Tx Buffer Add Request
    volatile uint32_t TXBCR;  // Tx Buffer Cancellation Request
    volatile uint32_t TXBTO;  // Tx Buffer Transmission Occurred
    volatile uint32_t TXBCF;  // Tx Buffer Cancellation Finished
    volatile uint32_t TXBTIE; // Tx Buffer Transmission Interrupt Enable
    volatile uint32_t TXBCIE; // Tx Buffer Cancellation Finished Interrupt Enable
    volatile uint32_t TXEFC;  // Tx Event FIFO Configuration
    volatile uint32_t TXEFS;  // Tx Event FIFO Status
    volatile uint32_t TXEFA;  // Tx Event FIFO Acknowledge
} FDCAN_TypeDef;

/* ========================================================================== */
/* MEMORY MAP                                     */
/* ========================================================================== */

#define PERIPH_BASE         0x40000000UL
#define APB1_BASE           0x40000000UL
#define APB2_BASE           0x40010000UL
#define AHB1_BASE           0x40020000UL
#define AHB2_BASE           0x42020000UL
#define AHB4_BASE           0x44020000UL

/* RCC */
#define RCC_BASE            (AHB4_BASE + 0x0C00UL)
#define RCC                 ((RCC_TypeDef *) RCC_BASE)

/* GPIO */
#define GPIO_BASE           (AHB2_BASE)
#define GPIO_PORT_OFFSET    0x0400UL
#define GPIO(bank)          ((GPIO_TypeDef *) (GPIO_BASE + (GPIO_PORT_OFFSET * (bank))))

/* USART */
#define USART3_BASE         (APB1_BASE + 0x4800UL) // 0x40004800
#define USART3              ((USART_TypeDef *) USART3_BASE)

/* SysTick */
#define SYSTICK_BASE        0xE000E010UL
#define SysTick             ((SysTick_TypeDef *) SYSTICK_BASE)

/* FDCAN */
#define FDCAN1_BASE         (0x4000A400UL)
#define FDCAN1              ((FDCAN_TypeDef *) FDCAN1_BASE)

/* ========================================================================== */
/* FUNCTION PROTOTYPES                              */
/* ========================================================================== */

// GPIO Modes
enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

void gpio_set_mode(uint16_t pin, uint8_t mode);
void gpio_write(uint16_t pin, bool val);
void gpio_toggle(uint16_t pin);
bool gpio_read(uint16_t pin);

void rcc_enable_bank(uint8_t pin);
void systick_init(uint32_t ticks);
void usart_init(void);

// Helper for printf (must be implemented by user)
void _putchar(char c);

#endif /* STM32H563_H */
