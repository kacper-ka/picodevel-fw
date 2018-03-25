
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common/custom_ops.h"
#include "common/xil_types.h"
#include "common/xgpio_l.h"

#ifndef SZ
#define SZ 128
#endif

static s32 m1[SZ][SZ] __attribute__((section(".shared")));
static s32 m2[SZ][SZ] __attribute__((section(".shared")));
static s32 m3[SZ][SZ] __attribute__((section(".shared")));
static u32 notdone __attribute__((section(".shared")));



static void matmul(s32 (*m1)[SZ][SZ], s32 (*m2)[SZ][SZ], s32 (*m3)[SZ][SZ]);

int main(void)
{
    u32 stamp0, stamp1;
    int row, col;
    
#if defined(WITH_FORK) && (WITH_FORK != 0)
	printf("System has %lu cores.\n", __CORES_COUNT__);
#else
    printf("Single core picorv32.\n");
#endif

    printf("Matrix size is %d x %d\n", SZ, SZ);
    printf("m1 @ 0x%08lX\n", (u32)&m1);
    printf("m2 @ 0x%08lX\n", (u32)&m2);
    printf("m3 @ 0x%08lX\n", (u32)&m3);
    
    srand(0x193F227C);
    __asm__ volatile ("rdcycle %0" : "=r" (stamp0));
	for (row = 0; row < SZ; row++)
	{
		for (col = 0; col < SZ; col++)
		{
			m1[row][col] = rand();
			m2[row][col] = rand();				
		}
	}
    __asm__ volatile ("rdcycle %0" : "=r" (stamp1));
    printf("Input arrays populated in %ld cycles.\n", stamp1-stamp0);
    
    __asm__ volatile ("rdcycle %0" : "=r" (stamp0));
	matmul(&m1, &m2, &m3);
    __asm__ volatile ("rdcycle %0" : "=r" (stamp1));
    printf("\nComputation complete in %ld cycles.\n", stamp1-stamp0);

    __sim_result = 123456789;
    XGpio_WriteReg(XPAR_AXI_GPIO_0_BASEADDR, XGPIO_DATA_OFFSET, 0xFF);
	return(0);     	
}

static void matmul(s32 (*m1)[SZ][SZ], s32 (*m2)[SZ][SZ], s32 (*m3)[SZ][SZ])
{
    int row, col, todo, i;
    s32 acc;
    unsigned job_size;
    
#if defined(WITH_FORK) && (WITH_FORK != 0)
    job_size = SZ / __CORES_COUNT__;
    while(__FORK() > 0);
    row = job_size * __get_COREID();
    todo = job_size;
#else
    todo = job_size = SZ;
    row = 0;
#endif
    
    while(todo > 0) {
        for(col = 0; col < SZ; col++) {
            for(acc = 0, i = 0; i < SZ; i++) {
                acc += (*m1)[row][i] * (*m2)[i][col];
            }
            (*m3)[row][col] = acc;
        }
        row++;
        todo--;
    }
    
#if defined(WITH_FORK) && (WITH_FORK != 0)
    __JOIN();
#endif

    notdone -= job_size;
    
#if defined(WITH_FORK) && (WITH_FORK != 0)
    if(__get_COREID() != 0) {
        __EXIT();
    }
#endif

    for (row = SZ - notdone; row < SZ; row++) {
        for (col = 0; col < SZ; col++) {
            for (acc = 0, i = 0; i < SZ; i++) {
                acc += (*m1)[row][i] * (*m2)[i][col];
            }
            (*m3)[row][col] = acc;
        }
        notdone--;
    }
    
}


