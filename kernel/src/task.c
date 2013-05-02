#include <task.h>
#include <heap.h>
#include <string.h>

extern bool sched_lock;
extern struct task_state *proc0;
extern struct task_state *currentprocess;
struct zombiepid *freepid ;
uint32_t zombiepid_counter = 0;
pid_t pid_counter = 0;



pid_t get_freepid(void)
{
    if(zombiepid_counter)
    {
        return zombiepid_get();
    }
    return pid_counter++;
}

pid_t proc_create(char* name, char* desc, pd_t *pd, prev_t prev, priority_t priority)
{
    struct task_state *proc = calloc(sizeof(proc[0]),1);
    proc->pagedir = pd;
    proc->flags  |= priority;
    strncpy(proc->name,name,256);
    strncpy(proc->desc,desc,256);
    while(sched_lock){}
    sched_lock = true;
    proc->next = proc0->next;
    proc->prev = proc0;
    proc0->next = proc;
    proc->pid = get_freepid();
    sched_lock = false;
    return proc->pid;
}

int proc_activate(pid_t pid)
{
    while(sched_lock){}
    sched_lock = true;
    struct task_state *temp = proc0->next;
    while(temp!=proc0)
    {
        if(temp->pid == pid)
        {
            temp->flags |= TASK_ACTIV;
            sched_lock = false;
            return 0;
        }
    }
    sched_lock = false;
    return 1;
}


void zombiepid_add(pid_t pid)
{
    while(sched_lock){}
    sched_lock = true;
    if(freepid)
    {
        struct zombiepid *newpid = malloc(sizeof(newpid[0]));
        newpid->next = freepid->next;
        newpid->prev = freepid;
        newpid->pid  = pid;
        freepid->next = newpid;
    }
    else
    {
        freepid = malloc(sizeof(freepid[0]));
        freepid->next = freepid;
        freepid->prev = NULL;
        freepid->pid = pid;
    }
    zombiepid_counter++;
    sched_lock = false;
}

pid_t zombiepid_get()
{
    while(sched_lock){}
    sched_lock = true;
    pid_t pid;
    if(freepid->next != freepid)
    {
        freepid->next->prev = NULL;
        pid = freepid->pid;
        struct zombiepid *next = freepid->next;
        free(freepid);
        freepid = next;
    }
    else
    {
        pid = freepid->pid;
        free(freepid);
        freepid = NULL;
    }
    zombiepid_counter--;
    sched_lock = false;
    return pid;
}


void proc_structure_free(struct task_state *proc)
{
    //warning destroys kernel PD!
    //pd_destroy(proc->pagedir);
    //destroy thread,zombietid,port and child list
    while(sched_lock){}
    sched_lock = true;
    zombiepid_add(proc->pid);
    proc->prev->next = proc->next;
    proc->next->prev = proc->prev;
    sched_lock = false;
    free(proc);
}

int proc_kill(struct task_state *proc)
{
    while(sched_lock){}
    sched_lock = true;
	proc->prev->next = proc->next;
	proc->next->prev = proc->prev;
	proc->flags = TASK_ZOMBIE;
	sched_lock = false;
	return 0;
}

void exit(int errorcode)
{
    proc_kill(currentprocess);
	asm volatile("int $32");
	while(1){}
}

struct task_state* proc_get(pid_t pid)
{
    struct task_state *temp = currentprocess;
    if(temp->pid == pid)
    {
        return temp;
    }
    temp = temp->next;
    while(temp!=currentprocess)
    {
        if(temp->pid == pid)
        {
            return temp;
        }
    }
    return NULL;
}





