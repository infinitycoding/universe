#include <task.h>
#include <heap.h>
#include <cpu.h>

extern bool sched_lock;

void zombietid_add(struct task_state *proc,tid_t tid)
{
    while(sched_lock){}
    sched_lock = true;
    if(proc->freetid)
    {
        struct zombietid *newtid = malloc(sizeof(newtid[0]));
        newtid->next = proc->freetid->next;
        newtid->prev = proc->freetid;
        newtid->tid = tid;
        proc->freetid->next = newtid;
    }
    else
    {
        proc->freetid = malloc(sizeof(proc->freetid[0]));
        proc->freetid->next = proc->freetid;
        proc->freetid->prev = NULL;
        proc->freetid->tid = tid;
    }
    sched_lock = false;
}

tid_t get_zombie_tid(struct task_state *proc)
{
    while(sched_lock){}
    sched_lock = true;
    tid_t tid;
    if(proc->freetid->next != proc->freetid)
    {
        proc->freetid->next->prev = NULL;
        tid = proc->freetid->tid;
        struct zombietid *next = proc->freetid->next;
        free(proc->freetid);
        proc->freetid = next;
    }
    else
    {
        tid = proc->freetid->tid;
        free(proc->freetid);
        proc->freetid = NULL;
    }
    sched_lock = false;
    return tid;
}

tid_t get_freetid(struct task_state *proc)
{
    if(proc->freetid)
    {
        return get_zombie_tid(proc);
    }
    return proc->tid_counter++;
}

tid_t thread_create(struct task_state *proc, uint32_t eip, void *arg)
{
    struct thread *newthread = malloc(sizeof(newthread[0]));
    if(proc->flags & TASK_KERNELMODE)
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

        newthread->thread_state = malloc(sizeof(new_state));
        *newthread->thread_state = new_state;
        newthread->flags = THREAD_ACTIV;
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

        newthread->thread_state = malloc(sizeof(new_state));
        *newthread->thread_state = new_state;
        newthread->flags = THREAD_ACTIV;
    }

    while(sched_lock){}
    sched_lock = true;
        if(proc->threads)
        {
            newthread->next = proc->threads;
            newthread->prev = proc->threads;
            proc->threads->prev = newthread;
            proc->threads->next = newthread;
        }
        else
        {
            newthread->next = newthread;
            newthread->prev = newthread;
            proc->threads = newthread;
            proc->currentthread = newthread;
        }
        newthread->tid = get_freetid(proc);
    sched_lock = false;
    return newthread->tid;
}

int thread_kill(struct thread *thr) {
return 0;
}

void thread_exit(int errorcode) {

}

struct thread *thread_get(tid_t tid) {
return NULL;
}

int thread_value(tid_t tid) {
return 0;
}
