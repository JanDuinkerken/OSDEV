#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_
#include "idt.h"

struct interrupt_frame;
__attribute__((interrupt)) void GenerixException_Handler(struct interrupt_frame *frame);

void init_interrupts();

#endif