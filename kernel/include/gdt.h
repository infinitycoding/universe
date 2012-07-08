#ifndef	_gdt_h_
#define	_gdt_h_

#include <stdint.h>

struct gdt_entry{
	uint16_t limit_low;
	uint16_t Base_low;
	uint8_t Base_middle;
	uint8_t Access;
	uint8_t limit_Flags;
	uint8_t base_high;
};

struct gdtpt{
	uint16_t limit;
	void* base;
} __attribute__((packed));

#endif