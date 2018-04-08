

RISCV_PREFIX ?= riscv32-unknown-elf-
HOST_PREFIX ?= arm-none-eabi-

TARGET_PICORV32 = timing-picorv32
TARGET_PICODEV = timing-picodev

BUILD_DIR ?= build
OBJ_DIR = obj
COMMON_DIR = ../common
PICODEV_LD = $(COMMON_DIR)/picodev.ld
PICODEV_LIB = -lcommon-picodev
PICORV32_LD = $(COMMON_DIR)/picorv32.ld
PICORV32_LIB = -lcommon-picorv32

DO_MAKEHEX = ../common/makehex.py

MATRIX_SZ ?= 50
MATMUL_SZ = -DSZ=$(MATRIX_SZ)
SRCS = main.c

OBJS = $(addprefix $(OBJ_DIR)/,$(addsuffix .o,$(basename $(SRCS))))
OBJ_DIRS = $(sort $(dir $(OBJS)))

.PHONY: all picorv32 picorv32-payload picodev picodev-payload clean
.PHONY: picorv32-disassembly picodev-disassembly
all: | $(BUILD_DIR)
	make picodev
	make picodev-payload
	make picodev-disassembly
	make clean
	make picorv32
	make picorv32-payload
	make picorv32-disassembly
	make clean

clean:
	rm -rf $(OBJ_DIR)

picodev: WITH_FORK=-DWITH_FORK=1
picodev: LINKER_SCRIPT=$(PICODEV_LD)
picodev: LIB=$(PICODEV_LIB)
picodev: $(BUILD_DIR)/$(TARGET_PICODEV).elf
picodev: $(BUILD_DIR)/$(TARGET_PICODEV).bin
picodev: $(BUILD_DIR)/$(TARGET_PICODEV).hex

picodev-payload: $(BUILD_DIR)/$(TARGET_PICODEV)-payload.o
picodev-disassembly: $(BUILD_DIR)/$(TARGET_PICODEV).disassembly

picorv32: LINKER_SCRIPT=$(PICORV32_LD)
picorv32: LIB=$(PICORV32_LIB)
picorv32: $(BUILD_DIR)/$(TARGET_PICORV32).elf
picorv32: $(BUILD_DIR)/$(TARGET_PICORV32).bin
picorv32: $(BUILD_DIR)/$(TARGET_PICORV32).hex

picorv32-payload: $(BUILD_DIR)/$(TARGET_PICORV32)-payload.o
picorv32-disassembly: $(BUILD_DIR)/$(TARGET_PICORV32).disassembly

$(OBJS): | $(OBJ_DIRS)

$(BUILD_DIR):
	mkdir -p $@

$(OBJ_DIRS):
	mkdir -p $@

$(OBJ_DIR)/%.o: %.c
	$(RISCV_PREFIX)gcc -c -march=rv32im -Os --std=c99 \
		-Wall -Wextra -pedantic -ffreestanding -nostdlib \
		$(WITH_FORK) $(MATMUL_SZ) -I".." -o $@ $<

$(BUILD_DIR)/%.elf: $(OBJS) $(LINKER_SCRIPT)
	@echo 'Linking $(notdir $(basename $@))...'
	$(RISCV_PREFIX)gcc -Os -ffreestanding -nostdlib $(WITH_FORK) \
		-Wl,-Bstatic,-T,$(LINKER_SCRIPT),-Map,$(addsuffix .map,$(basename $@)) \
		-Wl,--strip-debug,-gc-sections,-print-memory-usage \
		--specs=nano.specs -I".." -L"$(COMMON_DIR)" \
		-o $@ $(OBJS) -lc $(LIB) -lgcc
	@echo ' '

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(RISCV_PREFIX)objcopy -O binary -g $< $@
	@echo ' '

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.bin 
	$(DO_MAKEHEX) $< 16384 > $@
	@echo ' '

$(BUILD_DIR)/%-payload.o: $(BUILD_DIR)/%.bin
	$(HOST_PREFIX)objcopy -I binary -O elf32-littlearm -B arm \
		--redefine-sym _binary_$(BUILD_DIR)_$(subst -,_,$(basename $(notdir $<)))_bin_start=_bin_$(subst -,_,$(basename $(notdir $<)))_start \
		--redefine-sym _binary_$(BUILD_DIR)_$(subst -,_,$(basename $(notdir $<)))_bin_end=_bin_$(subst -,_,$(basename $(notdir $<)))_end \
		--redefine-sym _binary_$(BUILD_DIR)_$(subst -,_,$(basename $(notdir $<)))_bin_size=_bin_$(subst -,_,$(basename $(notdir $<)))_size \
		$< $@

$(BUILD_DIR)/%.disassembly: $(BUILD_DIR)/%.elf
	$(RISCV_PREFIX)objdump -xsd $< > $@
