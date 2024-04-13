#include "cpu.h"
#include "../arch/gdt.h"
#include "../arch/tss.h"
#include "../io/interrupts.h"
#include "../memory/heap.h"
#include "../memory/paging.h"
#include "../util/printf.h"
#include "../util/string.h"
extern void reloadGsFs();
extern void setGsBase(uint64_t base);

struct cpu *cpu;

void init_cpus() {
    // Only bsp will initialize the cpus until the last step
    // The write to goto_address makes the other cpus startup

    cpu = malloc(sizeof(struct cpu));
    memset(cpu, 0, sizeof(struct cpu));

    printf("Booting CPU %d\n", BSP_CPU);

    cpu->tss = get_tss(BSP_CPU);
    cpu->ctx = malloc(sizeof(struct cpu_context));
    memset(cpu->ctx, 0, sizeof(struct cpu_context));
    cpu->ustack = stackalloc(USER_STACK_SIZE) + USER_STACK_SIZE;
    memset(cpu->ustack - USER_STACK_SIZE, 0, USER_STACK_SIZE);
    cpu->kstack = stackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    memset(cpu->kstack - KERNEL_STACK_SIZE, 0, KERNEL_STACK_SIZE);

    tss_set_stack(cpu->tss, cpu->kstack, 0);
    tss_set_stack(cpu->tss, cpu->ustack, 3);

    reloadGsFs();
    setGsBase((uint64_t)cpu->ctx);

    load_gdt(BSP_CPU);
    load_interrupts_for_local_cpu();
    cpu->ready = 1;
}