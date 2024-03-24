#include "arch/simd.h"
#include "bootservices/bootservices.h"
#include "io/interrupts.h"
#include "memory/memory.h"
#include "util/printf.h"
#include "util/string.h"

void _start(void) {
    init_simd();
    init_memory();
    init_interrupts(1);

    printf("Hola Mundo\n");

    int *buffer = (int *)request_page();

    for (int i = 0; i < 100; i++) {
        buffer[i] = i;
    }
    for (int i = 0; i < 100; i++) {
        printf("buffer[%d] at %p = %d\n", i, &(buffer[i]), buffer[i]);
    }

    // Trigger a Divide by Zero exception
    int a = 5;
    int b = a / 0;
    printf("The result is %d\n", b);
    while (1)
        ;
}