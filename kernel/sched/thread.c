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
 *
 *  reviewed by Micha at 22.10.2014
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

// defined in sched/scheduler.c
extern struct thread_state *current_thread;
extern struct process_state *kernel_state;
extern list_t *running_threads;

/**
 * @brief 				create a new thread
 * @param process		the process of the thread to be created
 * @param prev			the privilege level of the new thread
 * @param eip			pointer to the entry
 * @param argv			argument values
 * @param environ		environment variables
 * @param return_adress	adress to jump after exiting
 */
struct thread_state *thread_create(struct process_state *process, privilege_t prev, vaddr_t eip, char **argv, char **environ, vaddr_t return_address, vmm_context_t *context)
{
    if(process == NULL)
    {
        // cannot create thread without process
        return NULL;
    }

    // allocate memory
    struct thread_state *new_thread = malloc(sizeof(struct thread_state));

    // process stuff
    new_thread->process = process;
    if(list_is_empty(process->zombie_tids))
        new_thread->tid = process->tid_counter++;
    else
        new_thread->tid = (tid_t) list_pop_back(process->zombie_tids);

    if(process->main_thread == NULL)
        process->main_thread = new_thread;

    // flags
    new_thread->ticks = 10;
    new_thread->flags = THREAD_ACTIVE;
    if(prev == KERNELMODE)
        new_thread->flags |= THREAD_KERNELMODE;

    // context
    new_thread->return_value = NULL;

    if(context != NULL)
        memcpy(&new_thread->context.memory.arch_context, &context->arch_context, sizeof(arch_vmm_context_t));
    else
        vmm_create_context(&new_thread->context.memory);

    thread_sync_context(new_thread);
    arch_create_thread_context(&new_thread->context, prev, eip, (vaddr_t) return_address, argv, environ);

    // heap
    if(process->heap_top == NULL)
    {
        process->heap_top = arch_vaddr_find(&new_thread->context.memory.arch_context, 1, MEMORY_LAYOUT_USER_HEAP_START, MEMORY_LAYOUT_USER_HEAP_END, VMM_PRESENT|VMM_WRITABLE|VMM_USER);
        vmm_map(&new_thread->context.memory, pmm_alloc_page(), process->heap_top, VMM_PRESENT|VMM_WRITABLE|VMM_USER);
        process->heap_lower_limit = process->heap_top;
        process->heap_upper_limit = (uint32_t) MEMORY_LAYOUT_STACK_TOP - 0x1000;
    }

    // add to thread lists
    list_push_front(running_threads, new_thread);
    list_push_front(process->threads, new_thread);

    return new_thread;
}

/**
 * @brief				clone a thread
 * @param process		the process for the new thread
 * @param src_thread	the original thread
 */
struct thread_state *thread_clone(struct process_state *process, struct thread_state *src_thread)
{
    if(process == NULL)
    {
        // cannot create thread without process
        return NULL;
    }

    // allocate memory
    struct thread_state *new_thread = malloc(sizeof(struct thread_state));

	// process stuff
    new_thread->process = process;
    if(list_is_empty(process->zombie_tids))
        new_thread->tid = process->tid_counter++;
    else
        new_thread->tid = (tid_t) list_pop_back(process->zombie_tids);

    if(process->main_thread == NULL)
        process->main_thread = new_thread;

	// flags
    new_thread->ticks = 10;
    new_thread->flags = THREAD_ACTIVE;

    privilege_t prev;
    if(src_thread->flags & THREAD_KERNELMODE)
    {
        prev = KERNELMODE;
        new_thread->flags |= THREAD_KERNELMODE;
    }
    else
        prev = USERMODE;


	// context
    new_thread->return_value = NULL;

    vmm_create_context(&new_thread->context.memory);
    arch_fork_context(&src_thread->context.memory.arch_context, &new_thread->context.memory.arch_context);
    thread_sync_context(new_thread);

    arch_create_thread_context(&new_thread->context, prev, (vaddr_t) 0, (vaddr_t) 0, 0, 0);

	// copy kernelstack
    memcpy((void*) new_thread->context.kernel_mode_stack, (void*) src_thread->context.kernel_mode_stack, 0x1000);

	// copy userstack
    if(prev == USERMODE)
    {
        paddr_t src_paddr = src_thread->context.program_stack;
        paddr_t dest_paddr = new_thread->context.program_stack;
        void *src_stack  = (void *) vmm_automap_kernel(current_context, src_paddr, VMM_PRESENT | VMM_WRITABLE);
        void *dest_stack = (void *) vmm_automap_kernel(current_context, dest_paddr, VMM_PRESENT | VMM_WRITABLE);

        memcpy(dest_stack, src_stack, 0x1000);

		vmm_unmap(current_context, src_stack);
		vmm_unmap(current_context, dest_stack);
    }

    if(process->heap_top == 0)
    {
        // what the fuck?
        process->heap_top = arch_vaddr_find(&new_thread->context.memory.arch_context, 1, MEMORY_LAYOUT_USER_HEAP_START, MEMORY_LAYOUT_USER_HEAP_END, VMM_PRESENT|VMM_WRITABLE|VMM_USER);
        vmm_map(&new_thread->context.memory, pmm_alloc_page(), process->heap_top, VMM_PRESENT|VMM_WRITABLE|VMM_USER);
        process->heap_lower_limit = process->heap_top;
        process->heap_upper_limit = (uint32_t) MEMORY_LAYOUT_STACK_TOP - 0x1000;
    }

    // add to thread lists
    list_push_front(process->threads,new_thread);
    list_push_front(running_threads, new_thread);

    return new_thread;
}

/**
 * @brief create a kernel-thread
 */
struct thread_state *kernel_thread_create(int (*thread)(int argc, char **argv, char **environ), char **argv, char **environ)
{
    struct thread_state *new_thread = thread_create(kernel_state, KERNELMODE, (vaddr_t) thread, argv, environ, (vaddr_t) &kernel_thread_exit, NULL);
    return new_thread;
}

/**
 * @brief			synchronize a threads context with the context of the main thread
 * @param thread	thread to synchronize
 */
void thread_sync_context(struct thread_state *thread)
{
    struct thread_state *main_thread = thread->process->main_thread;
    if(thread != main_thread && main_thread != NULL && thread != NULL)
    {
        int end = PDE_INDEX(0xB0000000);
        arch_sync_pts(&thread->context.memory.arch_context, &main_thread->context.memory.arch_context, 0, end);
    }
}

/**
 * @brief	exit the current thread in kernelmode
 */
void kernel_thread_exit(void)
{
    current_thread->flags |= THREAD_ZOMBIE;
    printf("death row\n");
    while(1) {}
    asm("int $32"); ///i don't know why ther's a pagefalut when i call the system scheduler
}


/**
 *  @brief			Suspends a running thread
 *  @param thread	pointer to the thread state
 */
void thread_suspend(struct thread_state *thread)
{
    if(! list_is_empty(running_threads))
    {
        iterator_t it = iterator_create(running_threads);
        list_set_first(&it);

        while(!list_is_last(&it))
        {
            if(list_get_current(&it) == thread)
            {
                list_remove(&it);
                thread->ticks = 0;
                thread->flags &= ~THREAD_ACTIVE;
            }
            list_next(&it);
        }
    }
}


/**
 * @brief			Wakes up a thread
 * @param thread	pointer to the thread state
 */
void thread_wakeup(struct thread_state *thread)
{
    thread->flags |= THREAD_ACTIVE;
    // the ticks will be set in the scheduler

    list_push_front(running_threads, thread);
}

/**
 * @brief			kills a thread. if its the current, define it as zombie
 * @param thread	thread to kill
 */
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

/**
 * @brief			destroys a thread and removes it from all lists
 * @param thread	thread to kill
 */
void thread_kill_sub(struct thread_state *thread)
{
    struct process_state *process = thread->process;

    // remove it from the global thread list
    if(thread == current_thread)
    {
        current_thread = NULL;
    }

    if(thread->flags & THREAD_ACTIVE || thread->flags & THREAD_ZOMBIE)
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
        // arch_destroy_thread_context(&thread->context); // FIXME
    }

    if(thread == thread->process->main_thread)
    {
        thread->process->main_thread = NULL;
    }

    //if(thread->process->flags & PROCESS_ZOMBIE)
    //{
    //    list_push_front(thread->process->zombie_tids,(void *) thread->tid);
    // remove it from process thread list
    iterator_t thread_it = iterator_create(process->threads);
    while(!list_is_last(&thread_it))
    {
        struct thread_state *t = list_get_current(&thread_it);
        if(t == thread)
        {
            list_remove(&thread_it);
            //if(list_is_empty(thread->process->threads))
            //    process_kill(thread->process);
            break;
        }
        list_next(&thread_it);
    }

    free(thread);
    //}
}



