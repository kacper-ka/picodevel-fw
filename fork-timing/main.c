
#include <stdio.h>
#include "common/custom_ops.h"
#include "common/xil_types.h"
#include "common/xgpio_l.h"

int main(void) {
#if defined(WITH_FORK) && (WITH_FORK != 0)
    u32 stamp0, stamp1;
    
    __asm__ volatile ("rdcycle %0" : "=r" (stamp0));
    __update_CORES_COUNT();
    __asm__ volatile ("rdcycle %0" : "=r" (stamp1));
    printf("Device has %lu cores, update took %ld cycles.\n", __CORES_COUNT__, stamp1 - stamp0);
    
    __asm__ volatile ("rdcycle %0" : "=r" (stamp0));
    if (__FORK() > 0) {
        __EXIT();
    }
    __asm__ volatile ("rdcycle %0" : "=r" (stamp1));
    printf("Single fork took %ld cycles.\n", stamp1 - stamp0);
#else
    printf("Nothing to test.\n");
#endif
    __sim_result = 123456789;
    XGpio_WriteReg(XPAR_AXI_GPIO_0_BASEADDR, XGPIO_DATA_OFFSET, 0xFF);
    return 0;
}