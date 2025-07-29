// syscalls.c - minimal syscalls for bare-metal STM32
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void _exit(int status) {
    while (1);
}

void *_sbrk(ptrdiff_t incr) {
    extern char _end;  // Provided by linker
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0)
        heap_end = &_end;

    prev_heap_end = heap_end;
    heap_end += incr;

    return (void *)prev_heap_end;
}

int _write(int fd, char *ptr, int len) {
    return len;
}

int _close(int fd) { return -1; }
int _fstat(int fd, struct stat *st) { return 0; }
int _lseek(int fd, int ptr, int dir) { return 0; }
int _read(int fd, char *ptr, int len) { return 0; }
int _isatty(int fd) { return 1; }

