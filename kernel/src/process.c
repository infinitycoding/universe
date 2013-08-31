#include <process.h>
#include <thread.h>
#include <heap.h>
#include <string.h>

extern pd_t *pd_kernel;
extern struct process_state *kernel_state;
extern struct thread_state *current_thread;

list_t *process_list = 0;
list_t *zombie_list = 0;
pid_t pid_counter = 1;


inline int min(int a, int b)
{
    return (a < b) ? a : b;
}
struct process_state *process_create(const char *name, const char *desc, uint16_t flags,struct process_state *parent)
{

    struct process_state *state = malloc(sizeof(state));
    int string_len = min(strlen(name), 255);
    state->name = (char *) malloc(string_len + 1);
    strncpy(state->name, name, string_len);
    state->name[string_len + 1] = 0;
    string_len = min(strlen(desc), 255);
    state->desc = (char *) malloc(string_len + 1);
    strncpy(state->desc, desc, string_len);
    state->desc[string_len + 1] = 0;

    state->pagedir = pd_create();

    int i;

    for (i = 0; i < 1024; i++)

    {
        state->pagedir->entries[i] = pd_kernel->entries[i];

    }

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
    list_push_front(process_list,state);
    asm volatile("sti");

    return state;
}


void process_kill(struct process_state *process)
{
    if(process->flags & PROCESS_ZOMBIE)
    {
        return;
    }

    process->flags = PROCESS_FREEZED;
    list_set_first(process->threads);
    bool still_running = false;
    asm volatile("cli");
    while(!list_is_last(process->threads))
    {
        struct thread_state *thread = list_get_current(process->threads);

        if(thread == current_thread)
        {
            thread->flags |= THREAD_ZOMBIE;
            still_running = true;
            list_next(process->threads);
        }
        else
        {
            free(thread->state);
            free(thread);
            list_remove(process->threads);
        }

    }
    asm volatile("sti");

    list_set_first(process->children);

    while(!list_is_empty(process->children))

    {
        struct child *current_child = list_pop_front(process->children);
        process_kill(current_child->process);
        free(current_child);
    }

    if(still_running == false)
    {
        list_push_front(zombie_list,(void*) process->pid);
        int i;

        for(i = 768; i < 1024; i++)
        {
            process->pagedir->entries[i] = 0;
        }

        free(process->name);
        free(process->desc);

        list_destroy(&process->ports);
        list_destroy(&process->threads);
        list_destroy(&process->zombie_tids);
        asm volatile("cli");
        list_set_first(process_list);

        while(!list_is_last(process_list))
        {
            if(list_get_current(process_list) == process)
            {
                list_remove(process_list);
                break;
            }
            list_next(process_list);
        }
        free(process);
        asm volatile("sti");
    }
    else
    {
        process->flags |= PROCESS_ZOMBIE;
    }
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
