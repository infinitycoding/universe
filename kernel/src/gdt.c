#include <gdt.h>

static struct gdt_entry GDT[7]; //Nulldesc,Datadesc0,Codedesc0,Datadesc3,Codedesc3,TSS,Callgate
static struct gdtpt gdtp;

void set_GDT_entry(int entry, uint32_t base, uint32_t size, uint8_t acess, uint8_t flags){
	GDT[entry].limit_low =(uint16_t)size;
	GDT[entry].Base_low =(uint16_t)base;
	GDT[entry].Base_middle =(uint8_t)(base>>16);
	GDT[entry].Access = acess;
	GDT[entry].limit_Flags=(uint8_t) ((flags<<4)|(size>>16));
	GDT[entry].base_high =(uint8_t) (base>>24);
}

void load_gdt(uint16_t last_entry){
	gdtp.limit = ((last_entry+1)*8)-1;
	gdtp.base = GDT;
	asm volatile("lgdt %0"::"m" (gdtp));
}

void INIT_GDT(void){
	set_GDT_entry(0,0,0,0,0);
	set_GDT_entry(1,0,0xFFFFF,0x9A,0xC);
	set_GDT_entry(2,0,0xFFFFF,0x92,0xC);
	set_GDT_entry(3,0,0xfffff,0xFA,0xC);
	set_GDT_entry(4,0,0xfffff,0xF2,0xC);
	load_gdt(4);
	asm volatile(
		"mov $0x10, %ax;"
		"mov %ax, %ds;"
		"mov %ax, %es;"
		"mov %ax, %fs;"
		"mov %ax, %gs;"
		"mov %ax, %ss;"
		"ljmp $0x8, $.1;"
		".1:;"
		);
}