#include <thread.h>
#include <heap.h>
#include <cpu.h>

extern list_t *running_threads;
extern struct thread_state* current_thread;

struct thread_state *thread_create(struct process_state *process, privilege_t prev, uint32_t eip, void *args)
{
    struct thread_state *new_thread = malloc(sizeof(struct thread_state));
    new_thread->flags = THREAD_ACTIV;
    new_thread->process = process;
    new_thread->ticks = 10;
    new_thread->return_value = 0;

    if(prev == KERNELMODE)
    {
        struct cpu_state new_state = {
            .gs = 0x10,
            .fs = 0x10,
            .es = 0x10,
            .ds = 0x10,
            .edi = 0,
            .esi = 0,
            .ebp = 0,
            .ebx = 0,
            .edx = 0,
            .ecx = 0,
            .eax = 0,
            .intr = 0,
            .error = 0,
            .eip = eip,
            .cs = 0x8,
            .eflags = 0x202,
        };

        new_thread->state = malloc(sizeof(new_state));
        *new_thread->state = new_state;
        new_thread->flags |= THREAD_KERNELMODE;
    }
    else
    {
        struct cpu_state new_state = {
            .gs = 0x2b,
            .fs = 0x2b,
            .es = 0x2b,
            .ds = 0x2b,
            .edi = 0,
            .esi = 0,
            .ebp = 0,
            .ebx = 0,
            .edx = 0,
            .ecx = 0,
            .eax = 0,
            .intr = 0,
            .error = 0,
            .eip = eip,
            .cs = 0x23,
            .eflags = 0x202,
            .esp = (uint32_t)malloc(THREAD_STACK_SIZE)+THREAD_STACK_SIZE,
            .ss = 0x2b,
        };

        new_thread->state = malloc(sizeof(new_state));
        *new_thread->state = new_state;
    }

    if(list_is_empty(process->zombie_tids))
        new_thread->tid = process->tid_counter++;
    else
        new_thread->tid = list_pop_back(process->zombie_tids);


    list_push_front(process->children,new_thread);
    list_push_front(running_threads, new_thread);
    return new_thread;
}

void thread_kill(struct thread_state *thread)
{
    asm volatile("cli");
    if(current_thread == thread)
        thread->flags |= THREAD_ZOMBIE;
    else
    {
        if(thread->flags & THREAD_ACTIV)
        {
            struct list_node *backup_current = running_threads->current;
            list_set_first(running_threads);
            while(!list_is_last(running_threads))
            {
                struct thread_state *t = list_get_current(running_threads);
                if(t == thread)
                {
                    list_remove(running_threads);
                    break;
                }
            }
            running_threads->current = backup_current;
        }
        free(thread->state);
        list_push_front(thread->process->zombie_tids,thread->tid);
        list_set_first(thread->process->threads);
        while(!list_is_last(thread->process->threads))
        {
            struct thread_state *t = list_get_current(thread->process->threads);
            if(t == thread)
            {
                list_remove(thread->process->threads);
                break;
            }
        }
        free(thread);
    }
    asm volatile("sti");
}



