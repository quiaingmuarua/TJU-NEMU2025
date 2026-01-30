#ifndef __IRQ_H__
#define __IRQ_H__

#include "common.h"

typedef struct TrapFrame {
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	int32_t irq;
	uint32_t error_code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t old_esp;
	uint32_t old_ss;
} TrapFrame;

#endif
