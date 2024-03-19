#include "bootservices/bootservices.h"
#include "util/printf.h"
#include "memory/memory.h"

void _start(void)
{
    printf("Hello, World!\n");
    init_memory();

    while (1)
        ;
}