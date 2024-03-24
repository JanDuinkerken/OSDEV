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

void _start(void) {
    init_simd();
    init_memory();
    init_interrupts();

    // test_memory_allocation();

    // Trigger a Page Fault exception
    printf("Attempting to write to a memory address that doesnt exist in physical memory\n");
    int* ptr = (int*)0xfffffffffffffffffffff;
    *ptr = 0;

    // Trigger a Divide by Zero exception
    int a = 5;
    int b = a / 0;
    printf("The result is %d\n", b);
    while (1)
        ;
}