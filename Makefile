################################################################################
# Makefile for STM32H563 FDCAN Project
################################################################################

# 1. Project Name and Target
TARGET = stm32h563_fdcan

# 2. Build Paths
BUILD_DIR = Build

# 3. Debugging and Optimization
DEBUG = 1
OPT = -Og

# 4. Source Files
# User sources
C_SOURCES =  \
Src/main.c \
Src/stm32h5xx_it.c \
Src/system_stm32h5xx.c \
Src/sysmem.c \
Src/syscalls.c \
Src/raceup_fdcan.c

# HAL Driver sources
C_SOURCES += \
Drivers/STM32H5xx_HAL_Driver/Src/stm32h5xx_hal.c \
Drivers/STM32H5xx_HAL_Driver/Src/stm32h5xx_hal_cortex.c \
Drivers/STM32H5xx_HAL_Driver/Src/stm32h5xx_hal_rcc.c \
Drivers/STM32H5xx_HAL_Driver/Src/stm32h5xx_hal_rcc_ex.c \
Drivers/STM32H5xx_HAL_Driver/Src/stm32h5xx_hal_gpio.c \
Drivers/STM32H5xx_HAL_Driver/Src/stm32h5xx_hal_dma.c \
Drivers/STM32H5xx_HAL_Driver/Src/stm32h5xx_hal_pwr.c \
Drivers/STM32H5xx_HAL_Driver/Src/stm32h5xx_hal_pwr_ex.c \
Drivers/STM32H5xx_HAL_Driver/Src/stm32h5xx_hal_flash.c \
Drivers/STM32H5xx_HAL_Driver/Src/stm32h5xx_hal_flash_ex.c \
Drivers/STM32H5xx_HAL_Driver/Src/stm32h5xx_hal_fdcan.c

# Startup file
ASM_SOURCES =  \
Startup/startup_stm32h563xx.s

# List object files
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# 5. Binaries and Tools
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
HEX = $(PREFIX)objcopy -O ihex
BIN = $(PREFIX)objcopy -O binary -S
STM32PRG = STM32_Programmer_CLI --verbosity 1 -c port=swd mode=HOTPLUG speed=Reliable

# 6. MCU Flags (Cortex-M33)
CPU = -mcpu=cortex-m33
FPU = -mfpu=fpv5-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# 7. Macros and Defines
C_DEFS =  \
-DSTM32H563xx \
-DUSE_HAL_DRIVER \
-DHSE_VALUE=8000000L

# 8. Includes
C_INCLUDES =  \
-IInc \
-IDrivers/CMSIS/Device/ \
-IDrivers/CMSIS/Core/ \
-IDrivers/STM32H5xx_HAL_Driver/Inc

# 9. Compiler Flags
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifneq ($(DEBUG), 0)
CFLAGS += -g -gdwarf-2
endif

# 10. Linker Flags
# Uses the file name you provided: STM32H563xx_FLASH.ld
LDSCRIPT = STM32H563xx_FLASH.ld
LIBS = -lc -lm -lnosys 
# Added mkdir here to ensure build dir exists before map file creation
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--gc-sections

# 11. Build Rules
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# Compile C
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	@echo "compiling $<"
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

# Assemble ASM
$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "assembling $<"
	@$(AS) -c $(CFLAGS) $< -o $@

# Link
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile | $(BUILD_DIR)
	@echo "linking $@"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@$(SZ) $@

# Convert
$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@$(BIN) $< $@
	
$(BUILD_DIR):
	mkdir -p $@

flash: $(BUILD_DIR)/$(TARGET).elf
	$(STM32PRG) --write $<
	$(STM32PRG) -hardRst

erase:
	$(STM32PRG) --erase all

reset:
	$(STM32PRG) -hardRst

clean:
	-rm -fR $(BUILD_DIR)
