#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include "device/port-io.h"

make_helper(nop) {
	print_asm("nop");
	return 1;
}

make_helper(int3) {
	void do_int3();
	do_int3();
	print_asm("int3");

	return 1;
}

make_helper(int_i_b) {
	uint8_t imm = instr_fetch(eip + 1, 1);

	current_sreg = R_SS;
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, cpu.eflags.val);
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, cpu.cs.selector);
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, eip + 2);

	GateDesc gate;
	lnaddr_t desc_addr = cpu.idtr.base + imm * 8;
	gate.part1 = lnaddr_read(desc_addr, 4);
	gate.part2 = lnaddr_read(desc_addr + 4, 4);

	uint32_t offset = gate.offset_15_0 | (gate.offset_31_16 << 16);
	cpu.cs.selector = gate.segment;
	sreg_load(R_CS);

	cpu.eip = offset - 2;
	print_asm("int $0x%x", imm);
	return 2;
}

make_helper(iret) {
	current_sreg = R_SS;
	uint32_t new_eip = swaddr_read(reg_l(R_ESP), 4);
	reg_l(R_ESP) += 4;
	cpu.cs.selector = swaddr_read(reg_l(R_ESP), 4);
	reg_l(R_ESP) += 4;
	cpu.eflags.val = swaddr_read(reg_l(R_ESP), 4);
	reg_l(R_ESP) += 4;

	sreg_load(R_CS);
	cpu.eip = new_eip - 1;
	print_asm("iret");
	return 1;
}

make_helper(cli) {
	cpu.eflags.IF = 0;
	print_asm("cli");
	return 1;
}

make_helper(sti) {
	cpu.eflags.IF = 1;
	print_asm("sti");
	return 1;
}

make_helper(hlt) {
	/* In NEMU, treat HLT as a no-op to keep execution going. */
	print_asm("hlt");
	return 1;
}

make_helper(pusha) {
	current_sreg = R_SS;
	uint32_t temp_esp = reg_l(R_ESP);

	reg_l(R_ESP) -= 4; swaddr_write(reg_l(R_ESP), 4, reg_l(R_EAX));
	reg_l(R_ESP) -= 4; swaddr_write(reg_l(R_ESP), 4, reg_l(R_ECX));
	reg_l(R_ESP) -= 4; swaddr_write(reg_l(R_ESP), 4, reg_l(R_EDX));
	reg_l(R_ESP) -= 4; swaddr_write(reg_l(R_ESP), 4, reg_l(R_EBX));
	reg_l(R_ESP) -= 4; swaddr_write(reg_l(R_ESP), 4, temp_esp);
	reg_l(R_ESP) -= 4; swaddr_write(reg_l(R_ESP), 4, reg_l(R_EBP));
	reg_l(R_ESP) -= 4; swaddr_write(reg_l(R_ESP), 4, reg_l(R_ESI));
	reg_l(R_ESP) -= 4; swaddr_write(reg_l(R_ESP), 4, reg_l(R_EDI));

	print_asm("pusha");
	return 1;
}

make_helper(popa) {
	current_sreg = R_SS;
	reg_l(R_EDI) = swaddr_read(reg_l(R_ESP), 4); reg_l(R_ESP) += 4;
	reg_l(R_ESI) = swaddr_read(reg_l(R_ESP), 4); reg_l(R_ESP) += 4;
	reg_l(R_EBP) = swaddr_read(reg_l(R_ESP), 4); reg_l(R_ESP) += 4;
	/* Skip ESP */
	reg_l(R_ESP) += 4;
	reg_l(R_EBX) = swaddr_read(reg_l(R_ESP), 4); reg_l(R_ESP) += 4;
	reg_l(R_EDX) = swaddr_read(reg_l(R_ESP), 4); reg_l(R_ESP) += 4;
	reg_l(R_ECX) = swaddr_read(reg_l(R_ESP), 4); reg_l(R_ESP) += 4;
	reg_l(R_EAX) = swaddr_read(reg_l(R_ESP), 4); reg_l(R_ESP) += 4;

	print_asm("popa");
	return 1;
}

make_helper(in_b_dx) {
	uint16_t port = reg_w(R_DX);
	reg_b(R_AL) = pio_read(port, 1);
	print_asm("inb (%%dx),%%al");
	return 1;
}

make_helper(in_v_dx) {
	uint16_t port = reg_w(R_DX);
	reg_l(R_EAX) = pio_read(port, 4);
	print_asm("inl (%%dx),%%eax");
	return 1;
}

make_helper(out_b_dx) {
	uint16_t port = reg_w(R_DX);
	pio_write(port, 1, reg_b(R_AL));
	print_asm("outb %%al,(%%dx)");
	return 1;
}

make_helper(out_v_dx) {
	uint16_t port = reg_w(R_DX);
	pio_write(port, 4, reg_l(R_EAX));
	print_asm("outl %%eax,(%%dx)");
	return 1;
}

make_helper(in_b_imm) {
	uint8_t port = instr_fetch(eip + 1, 1);
	reg_b(R_AL) = pio_read(port, 1);
	print_asm("inb $0x%x,%%al", port);
	return 2;
}

make_helper(in_v_imm) {
	uint8_t port = instr_fetch(eip + 1, 1);
	reg_l(R_EAX) = pio_read(port, 4);
	print_asm("inl $0x%x,%%eax", port);
	return 2;
}

make_helper(out_b_imm) {
	uint8_t port = instr_fetch(eip + 1, 1);
	pio_write(port, 1, reg_b(R_AL));
	print_asm("outb %%al,$0x%x", port);
	return 2;
}

make_helper(out_v_imm) {
	uint8_t port = instr_fetch(eip + 1, 1);
	pio_write(port, 4, reg_l(R_EAX));
	print_asm("outl %%eax,$0x%x", port);
	return 2;
}

make_helper(lea) {
	ModR_M m;
	m.val = instr_fetch(eip + 1, 1);
	int len = load_addr(eip + 1, &m, op_src);
	reg_l(m.reg) = op_src->addr;

	print_asm("leal %s,%%%s", op_src->str, regsl[m.reg]);
	return 1 + len;
}
