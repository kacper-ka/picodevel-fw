
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common/custom_ops.h"

#ifndef SZ
#define SZ 128
#endif

static int32_t m1[SZ][SZ] __attribute__((section(".shared")));
static int32_t m2[SZ][SZ] __attribute__((section(".shared")));
static int32_t m3[SZ][SZ] __attribute__((section(".shared")));
static uint32_t notdone __attribute__((section(".shared")));



static void matmul(int32_t (*m1)[SZ][SZ], int32_t (*m2)[SZ][SZ], int32_t (*m3)[SZ][SZ]);

int main(void)
{
    uint32_t stamp0, stamp1;
    int row, col;
    
#if defined(WITH_FORK) && (WITH_FORK != 0)
	printf("System has %lu cores.\n", __CORES_COUNT__);
#else
    printf("Single core picorv32.\n");
#endif

    printf("Matrix size is %d x %d\n", SZ, SZ);
    printf("m1 @ 0x%08lX\n", (uint32_t)&m1);
    printf("m2 @ 0x%08lX\n", (uint32_t)&m2);
    printf("m3 @ 0x%08lX\n", (uint32_t)&m3);
    
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
	return(0);     	
}

static void matmul(int32_t (*m1)[SZ][SZ], int32_t (*m2)[SZ][SZ], int32_t (*m3)[SZ][SZ])
{
    int row, col, todo, i;
    int32_t acc;
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


