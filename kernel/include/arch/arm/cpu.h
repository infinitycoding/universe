
#ifndef _cpu_h_
#define _cpu_h_
#include <stdint.h>


struct cpu_state
{
    // r13 und r14 des Usermodes
    uint32_t usr_r13, usr_r14;
    // CPSR vor dem IRQ und SPSR des Supervisormodes
    uint32_t cpsr, svc_spsr;
    // r13 und r14 des Supervisormodes
    uint32_t svc_r13, svc_r14;
    // r0 bis r12 sowie r15
    uint32_t r0, r1, r2, r3, r4, r5, r6, r7;
    uint32_t r8, r9, r10, r11, r12, r15;
} __attribute__((packed));

#endif