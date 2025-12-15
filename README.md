# STM32H563 Bare-Metal FDCAN Driver

## 1. Project Overview
This repository contains a bare-metal Hardware Abstraction Layer (HAL) and driver implementation for the **FDCAN (Flexible Data-Rate Controller Area Network)** peripheral on the **STM32H563ZI** microcontroller.

The project is developed for the **NUCLEO-H563ZI** development board without reliance on the STM32Cube HAL or LL libraries. It demonstrates low-level register manipulation, memory-mapped I/O configuration, and specific handling of the FDCAN Message RAM architecture required by the Cortex-M33 based STM32H5 series.

### Key Features
* **Zero-Overhead Driver**: direct register access for maximum performance and deterministic timing.
* **FDCAN Support**: configuration for Nominal and Data bit timings, Message RAM management, and TrustZone-compatible register access.
* **Debug Infrastructure**: integrated `printf` redirection via USART3 and SysTick-based timing.
* **Build System**: custom GNU Make-based build system supporting separate source/include directories and build artifact management.

---

## 2. Hardware Configuration

### 2.1 Development Board
* **Target Board**: NUCLEO-H563ZI (STM32H563ZIT6)
* **Core Architecture**: ARM Cortex-M33 with TrustZone

### 2.2 Pinout Configuration
The application utilizes the following GPIO assignments. Ensure external hardware (transceivers) is connected accordingly.

| Function | Peripheral | Pin Name | Nucleo Connector | Description |
| :--- | :--- | :--- | :--- | :--- |
| **CAN TX** | FDCAN1 | **PD1** | CN12 (Pin TBD) | FDCAN Transmit Line |
| **CAN RX** | FDCAN1 | **PD0** | CN12 (Pin TBD) | FDCAN Receive Line |
| **UART TX** | USART3 | **PD8** | CN9 (ST-Link VCP) | Debug Console Output (115200 baud) |
| **UART RX** | USART3 | **PD9** | CN9 (ST-Link VCP) | Debug Console Input |
| **LED 1** | GPIO | **PB0** | User LED 1 | Status Indicator (Green) |
| **LED 2** | GPIO | **PF4** | User LED 2 | Status Indicator (Yellow) |
| **LED 3** | GPIO | **PG4** | User LED 3 | Status Indicator (Red) |

**Note**: The FDCAN1 pins (PD0/PD1) operate at 3.3V logic levels. A compatible CAN transceiver (e.g., SN65HVD230) is required to interface with a physical CAN bus.

---

## 3. Software Architecture

The project adheres to a modular directory structure to separate hardware abstraction from application logic.

```text
CANDriver/
├── inc/                  # Public API Headers
│   ├── fdcan.h           # FDCAN Driver Interface
│   ├── stm32h563.h       # Register Definitions & Memory Map
│   └── printf.h          # Lightweight Printf Implementation
├── src/                  # Source Implementation
│   ├── main.c            # Application Entry Point & Logic
│   ├── fdcan.c           # FDCAN Driver Implementation
│   ├── stm32h563.c       # GPIO, RCC, USART, SysTick Impl
│   ├── syscalls.c        # Newlib System Call Stubs
│   └── printf.c          # Printf Implementation
├── startup/              # System Startup Code
│   └── startup.s         # Vector Table & Reset Handler (Assembly)
├── build/                # Build Artifacts (Auto-generated)
├── docs/                 # Documentation (Doxygen)
├── Makefile              # Build Configuration
└── linker.ld             # Linker Script
````

-----

## 4\. Build Instructions

### 4.1 Prerequisites

Ensure the following tools are installed and available in the system path:

1.  **ARM GCC Toolchain** (`arm-none-eabi-gcc`): Version 12.0 or higher recommended for Cortex-M33 support.
2.  **GNU Make**: For build automation.
3.  **STM32CubeProgrammer CLI**: For flashing the binary.
4.  **Doxygen** (Optional): For generating API documentation.

### 4.2 Compilation

To compile the project and generate the ELF binary:

```bash
# Build the project (default target)
make

# Clean build artifacts
make clean
```

The build output (ELF, Map file, Object files) will be located in the `build/` directory.

-----

## 5\. Flashing and Debugging

### 5.1 Flashing

The Makefile includes a target to flash the board using the ST-LINK interface via the `STM32_Programmer_CLI`.

```bash
make flash
```

*This command performs a hard reset after flashing.*

### 5.2 Debugging with GDB

To debug the application using GDB and OpenOCD/ST-LINK GDB Server:

1.  **Start the GDB Server** (Terminal 1):

    ```bash
    # Using OpenOCD (Recommended)
    openocd -f interface/stlink.cfg -f target/stm32h5x.cfg
    ```

2.  **Connect GDB** (Terminal 2):

    ```bash
    arm-none-eabi-gdb build/CANDriver.elf
    ```

3.  **GDB Commands**:

    ```gdb
    target remote localhost:3333
    monitor reset halt
    load
    break main
    continue
    ```

-----

## 6\. Documentation

The project uses Doxygen for inline API documentation.

### Generating Documentation

To generate the HTML reference manual:

```bash
doxygen Doxyfile
```

The output will be available in `docs/html/index.html`.

-----

## 7\. Licensing

This project is released under the MIT License. See the `LICENSE` file for details.

```
```
