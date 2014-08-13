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
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <printf.h>
#include <sched/thread.h>
#include <mm/heap.h>
#include <cpu.h>
#include <memory_layout.h>
#include <mm/paging.h>
#include <string.h>
#include <sched/scheduler.h>
#include <pmm.h>

extern list_t *running_threads;
extern struct thread_state* current_thread;
extern struct process_state *kernel_state;

void thread_sync_context(struct thread_state *thread)
{
    struct thread_state *main_thread = thread->process->main_thread;
    if(thread != main_thread && main_thread != NULL && thread != NULL)
    {
        int end = PDE_INDEX(0xB0000000);
        arch_sync_pts(&thread->context.memory.arch_context, &main_thread->context.memory.arch_context, 0, end);
    }
}

void kernel_thread_exit(void)
{
    current_thread->flags |= THREAD_ZOMBIE;
    printf("death row\n");
    while(1) {}
    asm("int $32"); ///i don't know why ther's a pagefalut when i call the system scheduler
}


struct thread_state *kernel_thread_create(uintptr_t eip, int argc, void **argv, char **environ)
{
    struct thread_state *new_thread = thread_create(kernel_state, KERNELMODE, (uint32_t) eip, argc, argv, environ, &kernel_thread_exit, NULL);

    return new_thread;
}


struct thread_state *thread_create(struct process_state *process, privilege_t prev, uint32_t eip, int argc, void **argv, char **environ, void *return_address, vmm_context_t *context)
{
    struct thread_state *new_thread = malloc(sizeof(struct thread_state));

    new_thread->flags = THREAD_ACTIV;
    new_thread->process = process;
    new_thread->ticks = 10;
    new_thread->return_value = 0;

    if(prev == KERNELMODE)
    {
        new_thread->flags |= THREAD_KERNELMODE;
        if(return_address == NULL)
            return_address = &kernel_thread_exit;
    }

    if(process->main_thread == NULL)
    {
        process->main_thread = new_thread;
    }

    vmm_create_context(&new_thread->context.memory);
    if(context != NULL)
        memcpy(&new_thread->context.memory.arch_context, &context->arch_context, sizeof(arch_vmm_context_t));
    thread_sync_context(new_thread);
    arch_create_thread_context(&new_thread->context, prev, (vaddr_t) eip, (vaddr_t) return_address, argc, argv, environ);

    if(process->heap_top == 0)
    {
        process->heap_top = arch_vaddr_find(&new_thread->context.memory.arch_context, 1, MEMORY_LAYOUT_USER_HEAP_START, MEMORY_LAYOUT_USER_HEAP_END, VMM_PRESENT|VMM_WRITABLE|VMM_USER);
        vmm_map(&new_thread->context.memory, pmm_alloc_page(), process->heap_top, VMM_PRESENT|VMM_WRITABLE|VMM_USER);
        process->heap_lower_limit = process->heap_top;
        process->heap_upper_limit = (uint32_t) MEMORY_LAYOUT_STACK_TOP - 0x1000;
    }

    if(list_is_empty(process->zombie_tids))
        new_thread->tid = process->tid_counter++;
    else
        new_thread->tid = (tid_t) list_pop_back(process->zombie_tids);

    list_push_front(process->threads,new_thread);
    list_push_front(running_threads, new_thread);


    return new_thread;
}

struct thread_state *thread_clone(struct process_state *process, struct thread_state *src_thread)
{
    struct thread_state *new_thread = malloc(sizeof(struct thread_state));
    new_thread->flags = THREAD_ACTIV;
    new_thread->process = process;
    new_thread->ticks = 10;
    new_thread->return_value = 0;
    privilege_t prev;

    if(src_thread->flags & THREAD_KERNELMODE)
    {
        prev = KERNELMODE;
        new_thread->flags |= THREAD_KERNELMODE;
    }
    else
        prev = USERMODE;

    if(process->main_thread == NULL)
    {
        process->main_thread = new_thread;
    }

    vmm_create_context(&new_thread->context.memory);
    memcpy(&new_thread->context.memory.arch_context, &src_thread->context.memory.arch_context, sizeof(arch_vmm_context_t));
    thread_sync_context(new_thread);
    arch_create_thread_context(&new_thread->context, prev, (vaddr_t) 0, (vaddr_t) 0, 0, 0, 0);
    *new_thread->context.state = *src_thread->context.state;

    if(process->heap_top == 0)
    {
        process->heap_top = arch_vaddr_find(&new_thread->context.memory.arch_context, 1, MEMORY_LAYOUT_USER_HEAP_START, MEMORY_LAYOUT_USER_HEAP_END, VMM_PRESENT|VMM_WRITABLE|VMM_USER);
        vmm_map(&new_thread->context.memory, pmm_alloc_page(), process->heap_top, VMM_PRESENT|VMM_WRITABLE|VMM_USER);
        process->heap_lower_limit = process->heap_top;
        process->heap_upper_limit = (uint32_t) MEMORY_LAYOUT_STACK_TOP - 0x1000;
    }

    if(list_is_empty(process->zombie_tids))
        new_thread->tid = process->tid_counter++;
    else
        new_thread->tid = (tid_t) list_pop_back(process->zombie_tids);

    list_push_front(process->threads,new_thread);
    list_push_front(running_threads, new_thread);

    return new_thread;
}


void thread_kill(struct thread_state *thread)
{
    disable_irqs();
    if(current_thread == thread)
        thread->flags |= THREAD_ZOMBIE;
    else
    {
        thread_kill_sub(thread);
    }
    enable_irqs();
}

void thread_kill_sub(struct thread_state *thread)
{
    if(thread->flags & THREAD_ACTIV || thread->flags & THREAD_ZOMBIE)
    {
        iterator_t running_thread_it = iterator_create(running_threads);
        while(!list_is_last(&running_thread_it))
        {
            struct thread_state *t = list_get_current(&running_thread_it);
            if(t == thread)
            {
                list_remove(&running_thread_it);
                break;
            }
            list_next(&running_thread_it);
        }
    }

    // only delete the cpu state of usermode threads. Freeing the kernel cpu-state can cause pagefaults
    if(! (thread->flags & THREAD_KERNELMODE))
    {
        arch_destroy_thread_context(&thread->context);
    }

    if(thread->process->flags & PROCESS_ZOMBIE)
    {
        list_push_front(thread->process->zombie_tids,(void *) thread->tid);
        iterator_t thread_it = iterator_create(thread->process->threads);
        while(!list_is_last(&thread_it))
        {
            struct thread_state *t = list_get_current(&thread_it);
            if(t == thread)
            {
                list_remove(&thread_it);
                if(list_is_empty(thread->process->threads))
                    process_kill(thread->process);
                break;
            }
            list_next(&thread_it);
        }
        free(thread);
    }
}

void thread_exit(struct cpu_state **cpu)
{
    current_thread->flags |= THREAD_ZOMBIE;
    *cpu = task_schedule(*cpu);
}

void launch_thread(struct cpu_state **cpu)
{
    thread_create(current_thread->process, USERMODE, (*cpu)->CPU_ARG1, (*cpu)->CPU_ARG2, (void**)(*cpu)->CPU_ARG3, (void*)(*cpu)->CPU_ARG4, NULL, NULL);
}


