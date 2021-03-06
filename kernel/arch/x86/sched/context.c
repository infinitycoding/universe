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
 *  @file /arch/x86/context.c
 *  @brief Management for architecuture dependent threads.
 *  Because Multitasking is based on switching between serveral CPU states and memory contexts
 *  there is a need for hardware abstraction. In ths file are functions defined which manage the
 *  creation, destruction and switching of these states and contexts.
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <arch_context.h>
#include <mm/layout.h>
#include <cpu.h>
#include <mm/pmm.h>
#include <string.h>
#include <mm/vmm.h>
#include <mm/heap.h>


/**
 *  @brief Creates a context for a new thread.
 *  @param context Pointer to an allocated memory area in which the context will be stored
 *  @param prev Previleg level of the new thread (kernelmode, usermode)
 *  @param entry Pointer to the programm init (_start or main, ...)
 *  @param return_adress (usermode only) The adress to wich the thread should return when it's work is done.
 *  @param argv Argument sting array
 *  @param environ String array of enviroent varrables. (Terminated by a NULL pointer)
 *  @return NULL if an error occured or the same addres as context
 */
struct cpu_state *arch_create_thread_context(struct arch_thread_context *context, privilege_t prev, vaddr_t entry,vaddr_t return_adress, char **argv, char **environ)
{
    void *kernel_stack = malloc(0x1000);
    struct cpu_state *new_state = kernel_stack + 0x1000 - sizeof(struct cpu_state) - 3*sizeof(uint32_t);

    context->kernel_mode_stack = (uint32_t)kernel_stack;
    context->state = new_state;

    memset(new_state, 0, sizeof(struct cpu_state));
    new_state->eip = entry;
    new_state->eflags = 0x00000202;

    size_t argc = 0;
    if(argv != NULL)
    {
        while(argv[argc] != NULL)
            argc++;
    }

    uint32_t *stack;
    if(prev == KERNELMODE)
    {
        new_state->cs = 0x08;
        new_state->ds = 0x10;
        new_state->es = 0x10;
        new_state->fs = 0x10;
        new_state->gs = 0x10;
        stack = kernel_stack + 0x1000;
        *--stack = (uint32_t) return_adress;
        *--stack = (uint32_t) environ;
        *--stack = (uint32_t) argv;
        *--stack = (uint32_t) argc;
    }
    else
    {
        vaddr_t *user_argv = NULL;
        if(argv != NULL)
        {
            size_t size = sizeof(char*);
            int i = 0;
            while(argv[i] != NULL)
            {
                size += sizeof(char*) + strlen(argv[i]) + 1;
                i++;
            }

            if(size > 0)
            {
                size_t pages = NUM_PAGES(size);

                paddr_t vars = pmm_alloc_page_range(pages);
                user_argv = (vaddr_t *)vmm_automap_user_range(&context->memory, vars, pages, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
                vaddr_t *kernel_argv = (vaddr_t *) vmm_automap_kernel_range(current_context, vars, pages, VMM_PRESENT | VMM_WRITABLE);
                vaddr_t vbase =(vaddr_t)( user_argv+((argc+1)*sizeof(char *)));

                char *user_strs = (char*) (kernel_argv+(argc+1)*sizeof(char *));
                for(i = 0; i < argc; i++)
                {
                    kernel_argv[i] = vbase;
                    strcpy(user_strs, argv[i]);

                    user_strs += strlen(argv[i])+sizeof(char);
                    vbase += strlen(argv[i])+sizeof(char);
                }
                kernel_argv[i] = 0;

                vmm_unmap(current_context, (vaddr_t) kernel_argv);
            }
        }

        vaddr_t *user_environ = NULL;
        if(environ != NULL)
        {
            size_t envc = 0;
            size_t size = sizeof(char*);
            while(environ[envc] != NULL)
            {
                size += sizeof(char*) + strlen(environ[envc]) + 1;
                envc++;
            }

            if(size > 0)
            {
                size_t pages = NUM_PAGES(size);

                paddr_t vars = pmm_alloc_page_range(pages);
                user_environ = (vaddr_t *)vmm_automap_user_range(&context->memory, vars, pages, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
                vaddr_t *kenrel_envp = (vaddr_t *) vmm_automap_kernel_range(current_context, vars, pages, VMM_PRESENT | VMM_WRITABLE);
                vaddr_t vbase =(vaddr_t)( user_environ+((envc+1)*sizeof(char *)));

                char *user_strs = (char*) (kenrel_envp+(envc+1)*sizeof(char *));
                int i;
                for(i = 0; i < envc; i++)
                {
                    kenrel_envp[i] = vbase;
                    strcpy(user_strs, environ[i]);

                    user_strs += strlen(environ[i])+sizeof(char);
                    vbase += strlen(environ[i])+sizeof(char);
                }
                kenrel_envp[i] = 0;

                vmm_unmap(current_context, (vaddr_t) kenrel_envp);
            }
        }




        paddr_t pframe = pmm_alloc_page();
        context->program_stack = pframe;
        vmm_map(&context->memory, pframe, MEMORY_LAYOUT_STACK_TOP-0x1000, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
        new_state->esp = (uint32_t) MEMORY_LAYOUT_STACK_TOP - 3*sizeof(uint32_t);

        stack = (uint32_t *) vmm_automap_kernel(current_context, pframe, VMM_PRESENT | VMM_WRITABLE);

        stack[1023] = (uint32_t) user_environ;
        stack[1022] = (uint32_t) user_argv;
        stack[1021] = (uint32_t) argc;
        stack[1020] = (uint32_t) return_adress;

        vmm_unmap(current_context, (vaddr_t)stack);

        new_state->cs = 0x1b;
        new_state->ss = 0x23;

        int i;
        for(i = 0; i < IO_BITMAP_LENGTH / 32; i++)
            context->ports.iobmp[i] = 0xFFFFFFFF;
    }

    return new_state;

}


/**
 *  @brief Destroys a thread context and frees its associated memory.
 *  @param context The context to destroy.
 */
void arch_destroy_thread_context(struct arch_thread_context *context)
{
    free(context->state);
    vmm_destroy_context(&context->memory);
    free(context);
}

