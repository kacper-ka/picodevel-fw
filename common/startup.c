
#include <stdlib.h>
#include "custom_ops.h"



static inline void
__attribute__((always_inline))
__init_data(unsigned int* from, unsigned int* region_begin,
		unsigned int* region_end)
{
	// Iterate and copy word by word.
	// It is assumed that the pointers are word aligned.
	unsigned int *p = region_begin;
	while (p < region_end)
		*p++ = *from++;
}

static inline void
__attribute__((always_inline))
__init_bss(unsigned int* region_begin, unsigned int* region_end)
{
	// Iterate and clear word by word.
	// It is assumed that the pointers are word aligned.
	unsigned int *p = region_begin;
	while (p < region_end)
		*p++ = 0;
}

// These magic symbols are provided by the linker.
extern void
(*__preinit_array_start[])(void) __attribute__((weak));
extern void
(*__preinit_array_end[])(void) __attribute__((weak));
extern void
(*__init_array_start[])(void) __attribute__((weak));
extern void
(*__init_array_end[])(void) __attribute__((weak));
extern void
(*__fini_array_start[])(void) __attribute__((weak));
extern void
(*__fini_array_end[])(void) __attribute__((weak));

// Iterate over all the preinit/init routines (mainly static constructors).
static inline void
__attribute__((always_inline))
__run_init_array(void)
{
	int count;
	int i;

	count = __preinit_array_end - __preinit_array_start;
	for (i = 0; i < count; i++)
		__preinit_array_start[i]();

	// If you need to run the code in the .init section, please use
	// the startup files, since this requires the code in crti.o and crtn.o
	// to add the function prologue/epilogue.

	count = __init_array_end - __init_array_start;
	for (i = 0; i < count; i++)
		__init_array_start[i]();
}

// Run all the cleanup routines (mainly static destructors).
static inline void
__attribute__((always_inline))
__run_fini_array(void)
{
	int count;
	int i;

	count = __fini_array_end - __fini_array_start;
	for (i = count; i > 0; i--)
		__fini_array_start[i - 1]();

	// If you need to run the code in the .fini section, please use
	// the startup files, since this requires the code in crti.o and crtn.o
	// to add the function prologue/epilogue.
	//_fini(); // DO NOT ENABE THIS!
}



extern unsigned int _fidata;
extern unsigned int _fdata;
extern unsigned int _edata;
extern unsigned int _fbss;
extern unsigned int _ebss;

extern int main(void);
extern void _exit(int);

#if defined(WITH_FORK) && (WITH_FORK != 0)
uint32_t __CORES_COUNT__ __attribute__((section(".shared.CORES_COUNT")));
#endif

void __attribute__((weak,noreturn))
__start(void)
{
    int code;
    __init_data(&_fidata, &_fdata, &_edata);
    __init_bss(&_fbss, &_ebss);
    
    __run_init_array();

#if defined(WITH_FORK) && (WITH_FORK != 0)
    // update cores count
    __CORES_COUNT__ = 0;
    while (__FORK() > 0);
    __JOIN();
    __CORES_COUNT__++;
    if (__get_COREID() != 0)
        __EXIT();
#endif

    code = main();

    __run_fini_array();

    _exit(code);
    for (;;);
}

int __attribute__((weak))
main(void)
{
    for(;;);
    return 0;
}

void __attribute__((weak,noreturn))
_exit(int code)
{
    (void)code;
    __EBREAK();
    for(;;);
}

uint32_t * __attribute__((weak))
__irq_handler(uint32_t* regs, uint32_t irq)
{
    (void)irq;
    return regs;
}
