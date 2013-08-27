#include <thread.h>
#include <heap.h>
#include <cpu.h>

extern list_t *running_threads;

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



}
/*
struct thread_state *thread_find(struct process_state *process,tid_t tid);
*/
