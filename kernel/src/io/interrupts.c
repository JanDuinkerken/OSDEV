#include "interrupts.h"
#include "../memory/memory.h"
#include "../util/string.h"
#include "../util/printf.h"
#include "../util/panic.h"

struct idtr idtr;

__attribute__((interrupt)) void GenericInterrupt_Handler(struct interrupt_frame *frame)
{
    (void)frame;
    panic("Generic interrupt handler");
}

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame *frame)
{
    (void)frame;
    panic("Page fault");
}

__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame *frame)
{
    (void)frame;
    panic("Double fault");
}

void set_idt_gate(uint64_t handler, uint8_t entry_offset, uint8_t type_attr, uint8_t selector)
{
    struct idtdescentry *interrupt = (struct idtdescentry *)(idtr.offset + entry_offset * sizeof(struct idtdescentry));
    memset(interrupt, 0, sizeof(struct idtdescentry));
    set_offset(interrupt, handler);
    interrupt->selector = selector;
    interrupt->type_attributes = type_attr;
}

void init_intetrrupt()
{
    __asm__("cli");

    idtr.limit = sizeof(struct idtdescentry) * 256 - 1;
    idtr.offset = (uint64_t)request_page();

    memset((void *)idtr.offset, 0, sizeof(struct idtdescentry) * 256);

    for (int i = 0; i < 256; i++)
    {
        set_idt_gate((uint64_t)GenericInterrupt_Handler, i, IDT_TA_InterruptGate, 0x28);
    }

    set_idt_gate((uint64_t)PageFault_Handler, 0x0E, IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)DoubleFault_Handler, 0x08, IDT_TA_InterruptGate, 0x28);

    __asm__ volatile("lidt %0" : : "m"(idtr));

    __asm__("sti");
}