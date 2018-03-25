
.PHONY: all clean purge
all:
	@make libcommon-picorv32.a
	@make libcommon-picodev.a

clean:
	rm -rf $(OBJ_DIR)

purge: clean
	rm *.a

RISCV_PREFIX ?= riscv32-unknown-elf-

OBJ_DIR = obj

WITH_FORK = 

SRCS = start.S startup.c xuartps_hw.c syscalls.c
OBJS = $(addprefix $(OBJ_DIR)/,$(addsuffix .o,$(basename $(SRCS))))


$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: %.S
	$(RISCV_PREFIX)gcc -c -march=rv32im $(WITH_FORK) -o $@ $<

$(OBJ_DIR)/%.o: %.c
	$(RISCV_PREFIX)gcc -c -march=rv32im -Os -std=c99 \
		-Wall -Wextra -pedantic -ffreestanding -nostdlib \
		-ffunction-sections -fdata-sections $(WITH_FORK) -o $@ $<

libcommon-picorv32.a: $(OBJS)
	@echo 'Packing archive...'
	$(RISCV_PREFIX)ar -rc $@ $(OBJS)
	@make clean

libcommon-picodev.a: WITH_FORK:=-DWITH_FORK=1
libcommon-picodev.a: $(OBJS)
	@echo 'Packing archive...'
	$(RISCV_PREFIX)ar -rc $@ $(OBJS)
	@make clean
