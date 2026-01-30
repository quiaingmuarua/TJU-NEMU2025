#include "common.h"
#include "memory.h"
#include <string.h>

static PDE updir[NR_PDE] align_to_page;
static CR3 ucr3;
static PTE uptables[NR_PDE][NR_PTE] align_to_page;
static uint32_t free_pa = 0;
static int nr_upt = 0;

PDE* get_updir() { return updir; }
uint32_t get_ucr3() { return ucr3.val; }

PDE* get_kpdir();

uint32_t cur_brk = 0;
// we do not free memory, so use `max_brk' to determine whether to call mm_malloc()
uint32_t max_brk = 0;

/* The brk() system call handler. */
uint32_t mm_brk(uint32_t new_brk) {
	if(new_brk != 0) {
		if(new_brk > max_brk) {
#ifdef IA32_PAGE
			mm_malloc(max_brk, new_brk - max_brk);
#endif
			max_brk = new_brk;
		}

		cur_brk = new_brk;
	}

	// If new_brk == 0, the brk() syscall should fail and return
	// the current break. See the NOTE of `man 2 brk' for details.

	return cur_brk;
}

void init_mm() {
	PDE *kpdir = get_kpdir();

	/* make all PDE invalid */
	memset(updir, 0, NR_PDE * sizeof(PDE));

	/* create the same mapping above 0xc0000000 as the kernel mapping does */
	memcpy(&updir[KOFFSET / PT_SIZE], &kpdir[KOFFSET / PT_SIZE], 
			(PHY_MEM / PT_SIZE) * sizeof(PDE));

	ucr3.val = (uint32_t)va_to_pa((uint32_t)updir) & ~0xfff;
	free_pa = KMEM;
	nr_upt = 0;
}

static PT alloc_pt() {
	assert(nr_upt < NR_PDE);
	PT pt = &uptables[nr_upt++];
	memset(pt, 0, PAGE_SIZE);
	return pt;
}

uint32_t mm_malloc(uint32_t vaddr, int len) {
	uint32_t start = vaddr & ~PAGE_MASK;
	uint32_t end = (vaddr + len + PAGE_MASK) & ~PAGE_MASK;

	for (uint32_t addr = start; addr < end; addr += PAGE_SIZE) {
		uint32_t pdir_idx = addr >> 22;
		uint32_t pte_idx = (addr >> 12) & 0x3ff;
		PT ptable;

		if (!updir[pdir_idx].present) {
			ptable = alloc_pt();
			updir[pdir_idx].val = make_pde(va_to_pa((uint32_t)ptable));
		} else {
			ptable = (PT)pa_to_va(updir[pdir_idx].page_frame << 12);
		}

		if (!(*ptable)[pte_idx].present) {
			assert(free_pa + PAGE_SIZE <= PHY_MEM);
			(*ptable)[pte_idx].val = make_pte(free_pa);
			free_pa += PAGE_SIZE;
		}
	}

	return vaddr;
}

