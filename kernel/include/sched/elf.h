#ifndef _ELF_H_
#define _ELF_H_

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the Universe Kernel.

     The Universe Kernel is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.

     The Universe Kernel is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with the Universe Kernel. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @author Michael Sippel (Universe Team) <micha.linuxfreak@gmail.com>
 */

#include <stdint.h>
#include <vfs/vfs.h>
#include <sched/process.h>

// File Types
#define ET_NONE		0x0 /* No type */
#define ET_REL		0x1 /* Relocatable file */
#define ET_EXEC		0x2 /* Executable file */
#define ET_DYN		0x3 /* Shared-Object-File */
#define ET_CORE		0x4 /* Core file */

// Machines
#define EM_NONE		0x00 /* None */
#define EM_M32		0x01 /* AT&T WE 32100 */
#define EM_SPARC	0x02 /* Sparc */
#define EM_386		0x03 /* Intel 80386 */
#define EM_68K		0x04 /* Motorola 68000 */
#define EM_88K		0x05 /* Motorola 88000 */
#define EM_860		0x06 /* Intel 80860 */
#define EM_MIPS		0x08 /* MIPS RS3000 */
#define EM_PPC		0x14 /* PowerPC */
#define EM_ARM		0x28 /* ARM */
#define EM_IA64		0x32 /* IA-64 */
#define EM_X86_64	0x3E /* AMD64 */

// Versions
#define ELF_VERSION_NONE 0 /* Invalid version */
#define ELF_VERSION_CURRENT 1 /* Current version */

// Indent-bytes
#define EI_NIDENT	16
#define EI_MAG0		0x0
#define EI_MAG1		0x1
#define EI_MAG2		0x2
#define EI_MAG3		0x3
#define EI_CLASS	0x4
#define EI_DATA		0x5
#define EI_VERSION	0x6
#define EI_PAD		0x7

// ELF-Magic
#define ELF_MAG0 0x7F
#define ELF_MAG1 'E'
#define ELF_MAG2 'L'
#define ELF_MAG3 'F'

struct elf_header
{
    uint8_t ident[EI_NIDENT];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;

    uint32_t ph_offset;
    uint32_t sh_offset;

    uint32_t flags;
    uint16_t header_size;

    uint16_t ph_entry_size;
    uint16_t ph_entry_count;
    uint16_t sh_entry_size;
    uint16_t sh_entry_count;
    uint16_t sh_str_table_index;
} __attribute__((packed));

// Program types
#define EPT_NULL	0x0 /* Invalid segment */
#define EPT_LOAD	0x1 /* Loadable segment */
#define EPT_DYN		0x2 /* Dynamic segment */
#define EPT_INTERP	0x3 /* Pos. of an str. that specifies the interpreter */
#define EPT_NOTE	0x4 /* Universal object */
#define EPT_SHLIB	0x5 /* Shared Libary */
#define EPT_PHDIR	0x6 /* pos. & size of programm-header */
#define EPT_TLS		0x7 /* Thread-Local Sotrage */

#define EP_FLAGS_X 0x1 /* Executable segment */
#define EP_FLAGS_W 0x2 /* Writable segment */
#define EP_FLAGS_R 0x3 /* Readable segment */

struct elf_program_header
{
    uint32_t type;
    uint32_t offset;
    uint32_t virt_addr;
    uint32_t phys_addr;
    uint32_t file_size;
    uint32_t mem_size;
    uint32_t flags;
    uint32_t alignment;
} __attribute__((packed));

struct process_state *load_elf_thread(void *image, struct process_state *proc, int argc, void **argv);
struct process_state *load_elf(void *image, char *name, uid_t uid, gid_t gid, struct pipeset *s);
struct process_state *load_elf_from_file(vfs_inode_t *inode, uid_t uid, gid_t gid, struct pipeset *s);

#endif
