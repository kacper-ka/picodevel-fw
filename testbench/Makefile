
TOOLCHAIN_PREFIX ?= /opt/picorv32/bin/riscv32-unknown-elf-

BUILD_DIR = build
OBJ_DIR = obj
COMMON_DIR = ../common
PICODEV_LD = $(COMMON_DIR)/picodev.ld
PICODEV_LIB = -lcommon-picodev
PICORV32_LD = $(COMMON_DIR)/picorv32.ld
PICORV32_LIB = -lcommon-picorv32

DO_MAKEHEX = ../common/makehex.py

TEST_DIR = tests
TEST_SRCS = $(wildcard $(TEST_DIR)/*.S)
TEST_OBJS = $(addprefix $(OBJ_DIR)/,$(addsuffix .o,$(basename $(TEST_SRCS))))

SRCS = \
	$(wildcard *.c) \
	$(wildcard *.S)

OBJS = $(addprefix $(OBJ_DIR)/,$(addsuffix .o,$(basename $(SRCS))))
OBJS += $(TEST_OBJS)
OBJ_DIRS = $(sort $(dir $(OBJS)))

WITH_FORK = -DWITH_FORK=0

.PHONY: all testbench-picodev testbench-picorv32 clean purge
all:
	make testbench-picorv32
	make clean
	make testbench-picodev
	make clean

testbench-picorv32: LINKER_SCRIPT=$(PICORV32_LD)
testbench-picorv32: LIB=$(PICORV32_LIB)
testbench-picorv32: $(BUILD_DIR)/tb-picorv32.elf
testbench-picorv32: $(BUILD_DIR)/tb-picorv32.bin
testbench-picorv32: $(BUILD_DIR)/tb-picorv32.hex

testbench-picodev: WITH_FORK=-DWITH_FORK=1
testbench-picodev: LINKER_SCRIPT=$(PICODEV_LD)
testbench-picodev: LIB=$(PICODEV_LIB)
testbench-picodev: $(BUILD_DIR)/tb-picodev.elf
testbench-picodev: $(BUILD_DIR)/tb-picodev.bin
testbench-picodev: $(BUILD_DIR)/tb-picodev.hex

clean:
	rm -rf $(OBJ_DIR)

purge:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)

$(OBJS): | $(OBJ_DIRS)

$(BUILD_DIR):
	mkdir -p $@

$(OBJ_DIRS):
	mkdir -p $@

# ---
# RISC-V tests objects
#
$(OBJ_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.S $(TEST_DIR)/riscv_test.h $(TEST_DIR)/test_macros.h
	$(TOOLCHAIN_PREFIX)gcc -c -march=rv32im -o $@ \
		-DTEST_FUNC_NAME=$(notdir $(basename $<)) \
		-DTEST_FUNC_TXT='"$(notdir $(basename $<))"' \
		-DTEST_FUNC_RET=$(notdir $(basename $<))_ret $<

# ---
# Testbench firmware sources
#
$(OBJ_DIR)/%.o: %.c
	$(TOOLCHAIN_PREFIX)gcc -c -march=rv32i -Os --std=c99 \
		-Wall -Wextra -pedantic -ffreestanding -nostdlib $(WITH_FORK) -o $@ $<

$(OBJ_DIR)/%.o: %.S
	$(TOOLCHAIN_PREFIX)gcc -c -march=rv32im $(WITH_FORK) -o $@ $<

# ---
# Testbench firmware output files
#
$(BUILD_DIR)/%.elf: $(OBJS) $(LINKER_SCRIPT) | $(BUILD_DIR)
	@echo 'Linking $(notdir $(basename $@))...'
	$(TOOLCHAIN_PREFIX)gcc -Os -ffreestanding -nostdlib $(WITH_FORK) \
		-Wl,-Bstatic,-T,$(LINKER_SCRIPT),-Map,$(addsuffix .map,$(basename $@)) \
		-Wl,--strip-debug,-gc-sections,-print-memory-usage \
		--specs=nano.specs \
		-o $@ $(OBJS) -I$(COMMON_DIR) -L$(COMMON_DIR) $(LIB) -lgcc
	@echo ' '

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(TOOLCHAIN_PREFIX)objcopy -O binary -g $< $@
	@echo ' '

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.bin 
	$(DO_MAKEHEX) $< 16384 > $@
	@echo ' '
	
#$(BUILD_DIR)/tb-picodev.elf: $(OBJS) $(PICODEV_LD) | $(BUILD_DIR)
#	@echo 'Linking picodev testbench firmware...'
#	$(TOOLCHAIN_PREFIX)gcc -Os -ffreestanding -nostdlib $(SYMBOLS) \
#		-Wl,-Bstatic,-T,$(PICODEV_LD),-Map,$(addsuffix .map,$(basename $@)) \
#		-Wl,--strip-debug,-gc-sections,-print-memory-usage \
#		--specs=nano.specs \
#		-o $@ $(OBJS) -I$(COMMON_DIR) -L$(COMMON_DIR) $(PICODEV_LIB) -lgcc
#	@echo ' '

#$(BUILD_DIR)/tb-picorv32.elf: $(OBJS) $(PICORV32_LD) | $(BUILD_DIR)
#	@echo 'Linking picorv32 testbench firmware...'
#	$(TOOLCHAIN_PREFIX)gcc -Os -ffreestanding -nostdlib $(SYMBOLS) \
#		-Wl,-Bstatic,-T,$(PICORV32_LD),-Map,$(addsuffix .map,$(basename $@)) \
#		-Wl,--strip-debug,-gc-sections,-print-memory-usage \
#		--specs=nano.specs \
#		-o $@ $(OBJS) -I"$(COMMON_DIR)" -L"$(COMMON_DIR)" -lgcc $(PICORV32_LIB)
#	@echo ' '
