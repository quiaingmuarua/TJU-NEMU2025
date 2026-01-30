#include "irq.h"

#include <sys/syscall.h>

void add_irq_handle(int, void (*)(void));
uint32_t mm_brk(uint32_t);
int fs_ioctl(int, uint32_t, void *);
void serial_printc(char);

static void sys_brk(TrapFrame *tf) {
	tf->eax = mm_brk(tf->ebx);
}

static void sys_ioctl(TrapFrame *tf) {
	tf->eax = fs_ioctl(tf->ebx, tf->ecx, (void *)tf->edx);
}

static void sys_write(TrapFrame *tf) {
	int fd = tf->ebx;
	const char *buf = (const char *)tf->ecx;
	uint32_t len = tf->edx;

	if (fd != 1 && fd != 2) {
		tf->eax = (uint32_t)-1;
		return;
	}

	for (uint32_t i = 0; i < len; i++) {
		serial_printc(buf[i]);
	}
	tf->eax = len;
}

static void sys_exit(TrapFrame *tf) {
	/* Use a good trap to stop NEMU when user program exits. */
	(void)tf->ebx; /* exit code ignored for now */
	HIT_GOOD_TRAP;
}

void do_syscall(TrapFrame *tf) {
	switch(tf->eax) {
		/* The `add_irq_handle' system call is artificial. We use it to
		 * let user program register its interrupt handlers. But this is
		 * very dangerous in a real operating system. Therefore such a
		 * system call never exists in GNU/Linux.
		 */
		case 0: 
			cli();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			sti();
			break;

		case SYS_write: sys_write(tf); break;
		case SYS_exit: sys_exit(tf); break;
		case SYS_brk: sys_brk(tf); break;
		case SYS_ioctl: sys_ioctl(tf); break;

		/* TODO: Add more system calls. */

		default: panic("Unhandled system call: id = %d, eip = 0x%08x", tf->eax, tf->eip);
	}
}
