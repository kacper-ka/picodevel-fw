// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

#define ENABLE_QREGS
#define ENABLE_RVTST
#define ENABLE_SIEVE
#define ENABLE_MULTST
#define ENABLE_STATS

#ifndef ENABLE_QREGS
#  undef ENABLE_RVTST
#endif

// Only save registers in IRQ wrapper that are to be saved by the caller in
// the RISC-V ABI, with the excpetion of the stack pointer. The IRQ handler
// will save the rest if necessary. I.e. skip x3, x4, x8, x9, and x18-x27.
#undef ENABLE_FASTIRQ


#define UART_BASE_ADDR  0xE0000000
#define UART_REG_SR     44
#define UART_REG_FIFO   48
#define UART_SR_TXFULL  0x10

#define RESULT_ADDR   0x00100000


#include "common/custom_ops.S"

	.section .text
	.global __start
	.global sieve
	.global multest
	.global hard_mul
	.global hard_mulh
	.global hard_mulhsu
	.global hard_mulhu
	.global stats
    .global outbyte
    .global puts
	.global __sim_result


/* Main program
 **********************************/

__start:
	jal ra,__init_data__
	jal ra,__init_bss__
	
#if defined(WITH_FORK) && (WITH_FORK != 0)
	addi x2,zero,-1
1:	picorv32_fork_insn(x1)
	beq x1,zero,2f // jump to join if fork successful
	beq x1,x2,3f // jump to epilogue if fork failed
	jal zero,1b // newly spawned process jumps to fork

2:	picorv32_join_insn()
	jal zero,3f
#endif

3:	addi x1,zero,1000
	picorv32_timer_insn(zero,x1)
	picorv32_waitirq_insn(zero)
	picorv32_maskirq_insn(zero, zero)

	/* zero-initialize all registers */

	addi x1, zero, 0
	addi x2, zero, 0
	addi x3, zero, 0
	addi x4, zero, 0
	addi x5, zero, 0
	addi x6, zero, 0
	addi x7, zero, 0
	addi x8, zero, 0
	addi x9, zero, 0
	addi x10, zero, 0
	addi x11, zero, 0
	addi x12, zero, 0
	addi x13, zero, 0
	addi x14, zero, 0
	addi x15, zero, 0
	addi x16, zero, 0
	addi x17, zero, 0
	addi x18, zero, 0
	addi x19, zero, 0
	addi x20, zero, 0
	addi x21, zero, 0
	addi x22, zero, 0
	addi x23, zero, 0
	addi x24, zero, 0
	addi x25, zero, 0
	addi x26, zero, 0
	addi x27, zero, 0
	addi x28, zero, 0
	addi x29, zero, 0
	addi x30, zero, 0
	addi x31, zero, 0

	/* running tests from riscv-tests */

#ifdef ENABLE_RVTST
#  define TEST(n) \
	.global n; \
	addi x1, zero, 1000; \
	picorv32_timer_insn(zero, x1); \
	jal zero,n; \
	.global n ## _ret; \
	n ## _ret:
#else
#  define TEST(n) \
	.global n ## _ret; \
	n ## _ret:
#endif

	TEST(lui)
	TEST(auipc)
	TEST(j)
	TEST(jal)
	TEST(jalr)

	TEST(beq)
	TEST(bne)
	TEST(blt)
	TEST(bge)
	TEST(bltu)
	TEST(bgeu)

	TEST(lb)
	TEST(lh)
	TEST(lw)
	TEST(lbu)
	TEST(lhu)

	TEST(sb)
	TEST(sh)
	TEST(sw)

	TEST(addi)
	TEST(slti) // also tests sltiu
	TEST(xori)
	TEST(ori)
	TEST(andi)
	TEST(slli)
	TEST(srli)
	TEST(srai)

	TEST(add)
	TEST(sub)
	TEST(sll)
	TEST(slt) // what is with sltu ?
	TEST(xor)
	TEST(srl)
	TEST(sra)
	TEST(or)
	TEST(and)

	TEST(mulh)
	TEST(mulhsu)
	TEST(mulhu)
	TEST(mul)

	TEST(div)
	TEST(divu)
	TEST(rem)
	TEST(remu)

	TEST(simple)

	/* set stack pointer */
	//lui sp,(64*1024)>>12
    lui sp,%hi(_estack)
    addi sp,sp,%lo(_estack)

	/* set gp and tp */
	lui gp, %hi(0xdeadbeef)
	addi gp, gp, %lo(0xdeadbeef)
	addi tp, gp, 0

#ifdef ENABLE_SIEVE
	/* call sieve C code */
	jal ra,sieve
#endif

#ifdef ENABLE_MULTST
	/* call multest C code */
	jal ra,multest
#endif

#ifdef ENABLE_STATS
	/* call stats C code */
	jal ra,stats
#endif

	/* print "DONE\n" */
	addi a0,zero,'D'
    jal  ra,outbyte
	addi a0,zero,'O'
    jal  ra,outbyte
	addi a0,zero,'N'
    jal  ra,outbyte
	addi a0,zero,'E'
    jal  ra,outbyte
	addi a0,zero,'\n'
    jal  ra,outbyte
	
	/* li a0, RESULT_ADDR */
	lui a0, %hi(__sim_result)
	addi a0, a0, %lo(__sim_result)
	li a1, 123456789
	sw a1,0(a0)
	/* sw a1,__sim_result */

	/* trap */
	ebreak


/* Hard mul functions for multest.c
 **********************************/

hard_mul:
	mul a0, a0, a1
	ret

hard_mulh:
	mulh a0, a0, a1
	ret

hard_mulhsu:
	mulhsu a0, a0, a1
	ret

hard_mulhu:
	mulhu a0, a0, a1
	ret

    
/* Print functions
 *****************/

outbyte:
    lui     a1,%hi(UART_BASE_ADDR)
    addi    a1,a1,%lo(UART_BASE_ADDR)
1:	lw	    a2,UART_REG_SR(a1)
	andi	a2,a2,UART_SR_TXFULL
	bne	    a2,zero,1b
	sb	    a0,UART_REG_FIFO(a1)
    ret
	

puts:
    addi    s1,ra,0
    addi    s2,a0,0
1:  lb      a0,0(s2)
    beq     a0,zero,2f
    jal     ra,outbyte
    addi    s2,s2,1
    jal     zero,1b
2:  addi    ra,s1,0
    ret

/* System init
 *****************/
__init_data__:
    lui     a0,%hi(_etext)
    addi    a0,a0,%lo(_etext)
    lui     a1,%hi(_fdata)
    addi    a1,a1,%lo(_fdata)
    lui     a2,%hi(_edata)
    addi    a2,a2,%lo(_edata)
1:  lw      a3,0(a0)
    sw      a3,0(a1)
    addi    a0,a0,4
    addi    a1,a1,4
    bne     a1,a2,1b
    ret

__init_bss__:
    lui     a0,%hi(_fbss)
    addi    a0,a0,%lo(_fbss)
    lui     a1,%hi(_ebss)
    addi    a1,a1,%lo(_ebss)
1:  sw      zero,0(a0)
    addi    a0,a0,4
    bne     a0,a1,1b
    ret
