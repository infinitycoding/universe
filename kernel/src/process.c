#include <process.h>
#include <thread.h>
#include <heap.h>
#include <string.h>
#include <scheduler.h>

extern pd_t *pd_kernel;
extern struct process_state *kernel_state;
extern struct thread_state *current_thread;
extern list_t *running_threads;

list_t *process_list = 0;
list_t *zombie_list = 0;
pid_t pid_counter = 1;


inline int min(int a, int b)
{
    return (a < b) ? a : b;
}

struct process_state *process_create(const char *name, const char *desc, uint16_t flags,struct process_state *parent)
{

    struct process_state *state = malloc(sizeof(struct process_state));
    int string_len = min(strlen(name), 255);
    state->name = (char *) malloc(string_len + 1);
    strncpy(state->name, name, string_len);
    state->name[string_len + 1] = 0;
    string_len = min(strlen(desc), 255);
    state->desc = (char *) malloc(string_len + 1);
    strncpy(state->desc, desc, string_len);
    state->desc[string_len + 1] = 0;
    state->flags = flags;
    state->children = list_create();
    state->zombie_tids = list_create();
    state->threads = list_create();
    state->ports = list_create();
    state->tid_counter = 0;

    if (parent == NULL)
        state->parent = kernel_state;
    else
        state->parent = parent;

    if (list_is_empty(zombie_list))
    {
        state->pid = pid_counter++;
    }
    else
    {
        state->pid = (pid_t) list_pop_front(zombie_list);
    }


    asm volatile("cli");

        if(state->pid != 1)
        {
            struct child *new_child = malloc(sizeof(struct child));
            new_child->process = state;
            new_child->return_value = 0;
            list_push_front(state->parent->children, new_child);
        }

        list_push_front(process_list, state);

    asm volatile("sti");

    return state;
}


void process_kill(struct process_state *process)
{
    asm volatile("cli");
    list_set_first(process->threads);

    while(!list_is_empty(process->threads))
    {
        struct thread_state *thread = list_pop_front(process->threads);
        if(thread == current_thread)
        {
            current_thread->flags |= THREAD_ZOMBIE;
            process->flags |= PROCESS_ZOMBIE;
        }
        else
            thread_kill_sub(thread);
    }

    list_set_first(process->children);
    while(!list_is_empty(process->children))
    {
        struct child *current_child = list_pop_front(process->children);
        process_kill(current_child->process);
        free(current_child);
    }

    list_destroy(&process->ports);
    list_destroy(&process->zombie_tids);

    list_set_first(process_list);

    while(!list_is_last(process_list))
    {
        if(((struct process_state *)list_get_current(process_list)) == process)
        {
            list_remove(process_list);
            break;
        }
        list_next(process_list);
    }

    free(process->name);
    free(process->desc);

    if(! (process->flags & PROCESS_ZOMBIE) )
    {
        free(process);
        list_set_first(running_threads);
        current_thread = list_get_current(running_threads);
    }
    asm volatile("sti");
}


struct process_state *process_find(pid_t pid)
{
    list_set_first(process_list);
    while(!list_is_last(process_list))
    {
        if(((struct process_state *)list_get_current(process_list))->pid == pid)
        {
            return (struct process_state *)list_get_current(process_list);
        }
        list_next(process_list);
    }
    return 0;
}


void exit(struct cpu_state **cpu)
{
    list_set_first(current_thread->process->parent->children);
    while(list_is_last(current_thread->process->parent->children))
    {
        struct child *current_child = list_get_current(current_thread->process->parent->children);
        if(current_child->process == current_thread->process)
        {
            current_child->return_value = (*cpu)->ebx;
            break;
        }
        list_next(current_thread->process->parent->children);
    }

    process_kill(current_thread->process);
    *cpu = task_schedule(*cpu);
}


void fork(struct cpu_state **cpu)
{
    printf("sorry! this function is not implemented yet... :(\n");
}
