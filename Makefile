################################################################################
# PROJECT CONFIGURATION
################################################################################
TARGET = CANDriver
BUILD_DIR = build

# Source and Header Directories
SRC_DIR = src
INC_DIR = inc
DRIVERS_SRC_DIR = src/drivers
DRIVERS_INC_DIR = inc/drivers

STARTUP_DIR = startup

# Debug configuration (1=Enable, 0=Disable)
debug = 1

# FPU Configuration (hard/softfp/soft)
fpu = hard

################################################################################
# TOOLS
################################################################################
CC = arm-none-eabi-gcc
SZ = arm-none-eabi-size
# STM32 Programmer CLI
STM32PRG = STM32_Programmer_CLI --verbosity 1 -c port=swd mode=HOTPLUG speed=Reliable

################################################################################
# SOURCES
################################################################################
# Recursively find all .c files in src/ and its subdirectories
C_SOURCES = $(shell find $(SRC_DIR) -name '*.c')
ASM_SOURCES = $(wildcard $(STARTUP_DIR)/*.s)

# Generate list of object files in the build directory
# We flatten the structure for objects or keep it. 
# For simplicity in this makefile, we'll flatten the object paths in build/
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))

# VPATH tells 'make' where to look for source files
# We need to add all directories containing source files
SOURCE_DIRS = $(sort $(dir $(C_SOURCES)))
VPATH = $(SOURCE_DIRS):$(STARTUP_DIR)

################################################################################
# COMPILER FLAGS
################################################################################
# Optimization & Debugging
ifeq ($(debug), 1)
	DEBUGFLAGS = -g3 -O0
else
	DEBUGFLAGS = -O2
endif

# FPU Settings
ifeq ($(fpu), softfp)
	FLOATFLAGS = -mfloat-abi=softfp -mfpu=fpv5-sp-d16
else ifeq ($(fpu), hard)
	FLOATFLAGS = -mfloat-abi=hard -mfpu=fpv5-sp-d16
else
	FLOATFLAGS = -mfloat-abi=soft
endif

# Core Flags
MCU_FLAGS = -mcpu=cortex-m33 -mthumb $(FLOATFLAGS)

# Compiler Flags
CFLAGS = $(MCU_FLAGS) -std=gnu11 -Wall -fdata-sections -ffunction-sections
CFLAGS += $(DEBUGFLAGS)
CFLAGS += -I$(INC_DIR)
CFLAGS += -I$(DRIVERS_INC_DIR)
CFLAGS += --specs=nano.specs

# Printf Configuration flags
CFLAGS += -DPRINTF_DISABLE_SUPPORT_FLOAT -DPRINTF_DISABLE_SUPPORT_EXPONENTIAL -DPRINTF_DISABLE_SUPPORT_LONG_LONG

# Assembler Flags
ASFLAGS = $(MCU_FLAGS) -x assembler-with-cpp $(DEBUGFLAGS)

# Linker Flags
LDFLAGS = $(MCU_FLAGS)
LDFLAGS += -T"linker.ld"
LDFLAGS += --specs=nosys.specs 
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map
LDFLAGS += -static --specs=nano.specs
LDFLAGS += -Wl,--start-group -lc -lm -Wl,--end-group

################################################################################
# BUILD RULES
################################################################################

all: $(BUILD_DIR)/$(TARGET).elf

$(BUILD_DIR):
	mkdir -p $@

# Compile C sources
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "Compiling $<..."
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

# Assemble ASM sources
$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "Assembling $<..."
	@$(CC) -c $(ASFLAGS) $< -o $@

# Link the final executable
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) linker.ld Makefile | $(BUILD_DIR)
	@echo "Linking $@..."
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@$(SZ) $@

flash: $(BUILD_DIR)/$(TARGET).elf
	$(STM32PRG) --write $<
	$(STM32PRG) -hardRst

erase:
	$(STM32PRG) --erase all

reset:
	$(STM32PRG) -hardRst

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean flash erase reset
