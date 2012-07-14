#define ATTACHED	1
#define WRITABLE	2
#define ACCESSABLE 	4
#define WTC			8
#define NOCACHE		16
#define USED		32
#define WRITEPAGE	64
#define BIGPAGE		128
#define BIT_G		256
#define IGNORE_TLB	256

typedef int ptable_t;
typedef int pentry_t;

ptable_t vmm_pdir[1024];

int INIT_VMM()
{
	/* install page directory */
	asm (
		"mov %0, %eax" : : "q" (vmm_pdir)
		"shl %eax, $12"
		"mov %eax, %cr3"
	);
	
	/* activate paging */
	asm (
		"mov %cr0, %eax"
		"or %eax, $1"
		"shr %eax, $1"
	);
	
	return 0;
}

ptable_t vmm_alloc_ptable()
{
	ptable_t table;
	table |= WRITABLE;
	
	return table;
}
