
.PHONY: all clean purge
all:
	@make libcommon-picorv32.a
	@make libcommon-picodev.a

clean:
	rm -rf $(OBJ_DIR)

purge: clean
	rm *.a

TOOLCHAIN_PREFIX ?= /opt/picorv32/bin/riscv32-unknown-elf-

OBJ_DIR = obj

WITH_FORK = 

SRCS = start.S startup.c xuartps_hw.c
OBJS = $(addprefix $(OBJ_DIR)/,$(addsuffix .o,$(basename $(SRCS))))


$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: %.S
	$(TOOLCHAIN_PREFIX)gcc -c -march=rv32im $(WITH_FORK) -o $@ $<

$(OBJ_DIR)/%.o: %.c
	$(TOOLCHAIN_PREFIX)gcc -c -march=rv32im -Os -std=c99 \
		-Wall -Wextra -pedantic -ffreestanding -nostdlib \
		-ffunction-sections -fdata-sections $(WITH_FORK) -o $@ $<

libcommon-picorv32.a: $(OBJS)
	@echo 'Packing archive...'
	$(TOOLCHAIN_PREFIX)ar -rc $@ $(OBJS)
	@make clean

libcommon-picodev.a: WITH_FORK:=-DWITH_FORK=1
libcommon-picodev.a: $(OBJS)
	@echo 'Packing archive...'
	$(TOOLCHAIN_PREFIX)ar -rc $@ $(OBJS)
	@make clean