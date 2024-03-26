#include "arch/simd.h"
#include "bootservices/bootservices.h"
#include "io/interrupts.h"
#include "memory/memory.h"
#include "util/printf.h"
#include "util/string.h"

void test_memory_allocation() {
    int *buffer = (int *)request_page();

    for (int i = 0; i < 100; i++) {
        buffer[i] = i;
    }
    for (int i = 0; i < 100; i++) {
        printf("buffer[%d] at %p = %d\n", i, &(buffer[i]), buffer[i]);
    }
}

void trigger_page_fault() {
    int *ptr = (int *)0x0;
    *ptr = 0xdeadbeef;
}

void trigger_divide_by_zero() {
    int a = 5;
    int b = a / 0;
    printf("The result is %d\n", b);
}

void _start(void) {
    init_simd();
    init_memory();
    init_interrupts();

    // test_memory_allocation();
    trigger_page_fault();
    // trigger_divide_by_zero();
    while (1)
        ;
}