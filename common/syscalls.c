

#include "custom_ops.h"
#include "xuartps_hw.h"

#include <sys/stat.h>
#include <sys/times.h>




/*
environ

    A pointer to a list of environment variables and their values. For a minimal environment, this empty list is adequate:
*/
char *__env[1] = { 0 };
char **environ = __env;


/*
isatty

    Query whether output stream is a terminal. For consistency with the other minimal implementations, which only support output to stdout, this minimal implementation is suggested:
*/
int _isatty(int file)
{
    (void)file;
    return -1;
}

/*
_open_r

    A reentrant version of open. It takes a pointer to the global data block, which holds errno.
*/
int _open(const char *file, int flags, int mode)
{
    (void)file; (void)flags; (void)mode;
    return -1;
}

/*
_close_r

    A reentrant version of close. It takes a pointer to the global data block, which holds errno.
*/
int _close(int fd)
{
    (void)fd;
    return -1;
}

/*
_lseek_r

    A reentrant version of lseek. It takes a pointer to the global data block, which holds errno.
*/
off_t _lseek(int fd, off_t pos, int whence)
{
    (void)fd; (void)pos; (void)whence;
    return -1;
}

/*
_read_r

    A reentrant version of read. It takes a pointer to the global data block, which holds errno.
*/
long _read(int fd, void *buf, size_t cnt)
{
    (void)fd; (void)buf; (void)cnt;
    return -1;
}

/*
_write_r

    A reentrant version of write. It takes a pointer to the global data block, which holds errno.
*/
long _write(int fd, const void *buf, size_t cnt)
{
    (void)fd;
    size_t offs;
    for (offs = 0; offs < cnt; offs++) {
        XUartPs_SendByte(XPS_UART0_BASEADDR, ((uint8_t*)buf)[offs]);
    }
    return cnt;
}


int _fstat(int fd, struct stat *pstat)
{
    (void)fd; (void)pstat;
    return -1;
}


char *_sbrk(size_t incr)
{
    extern char _sheap;		/* Defined by the linker */
    extern char _fstack;
    static char *heap_end;
    char *prev_heap_end;
    
    if (heap_end == 0)
    {
        heap_end = &_sheap;
    }
    prev_heap_end = heap_end;
    incr = (incr + 3) & ~3;
    if (heap_end + incr > &_fstack)
    {
        _write(1, "Heap and stack collision\n", 25);
        __EBREAK();
        for(;;);
    }

    heap_end += incr;
    return prev_heap_end;
}

