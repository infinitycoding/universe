#include <process.h>
#include <thread.h>
#include <heap.h>
#include <string.h>
#include <scheduler.h>
#include <paging.h>
#include <memory_layout.h>
#include <trigger.h>

extern struct process_state *kernel_state;
extern struct thread_state *current_thread;
extern list_t *running_threads;
extern vfs_inode_t *root;


list_t *process_list = 0;
list_t *zombie_list = 0;
pid_t pid_counter = 1;

/**
 * returns the smaller inputvalue
 * @param a     inputvalue
 * @param b     inputvalue
 * @return the smallest inputvalue
 */
inline int min(int a, int b)
{
    return (a < b) ? a : b;
}

void dump_thread_list(list_t *threads)
{
    list_set_first(threads);
    printf("-----%d-----\n",list_length(threads));
    while(!list_is_empty(threads) && !list_is_last(threads))
    {
        struct thread_state *t = list_get_current(threads);
        printf("PID: %d  TID: %d  FLAG: %08x\n",t->process->pid, t->tid, t->flags);
        list_next(threads);
    }
    list_set_first(threads);
}


/**
 * creates a process
 * @param name      name of the process (max. 255 characters)
 * @param desc      description of the process (max. 255 characters)
 * @param flags     process flags (activ, freezed, zombies)
 * @param parent    pointer to the parent process struct (NULL: Kernel Init = parent)
 * @return
 */
struct process_state *process_create(const char *name, const char *desc, uint16_t flags,struct process_state *parent,vfs_inode_t *stdin,vfs_inode_t *stdout,vfs_inode_t *stderr)
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
    state->files = list_create();
    state->cwd = root;
    state->children = list_create();
    state->zombie_tids = list_create();
    state->threads = list_create();
    state->ports = list_create();
    state->tid_counter = 1;

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
            new_child->status = 0;
            list_push_front(state->parent->children, new_child);
        }

        list_push_front(process_list, state);

    asm volatile("sti");


    // create stream files
    if(stdin == NULL) stdin = vfs_create_pipe();
    if(stdout == NULL) stdout = vfs_create_pipe();
    if(stderr == NULL) stderr = vfs_create_pipe();

    struct fd *desc0 = malloc(sizeof(struct fd));
    desc0->id = 0;
    desc0->mode = 0x7ff;
    desc0->flags = O_RDONLY;
    desc0->pos = 0;
    desc0->inode = stdin;
    list_push_back(state->files, desc0);

    struct fd *desc1 = malloc(sizeof(struct fd));
    desc1->id = 1;
    desc1->mode = 0x7ff;
    desc1->flags = O_WRONLY;
    desc1->pos = 0;
    desc1->inode = stdout;
    list_push_back(state->files, desc1);

    struct fd *desc2 = malloc(sizeof(struct fd));
    desc2->id = 2;
    desc2->mode = 0x7ff;
    desc2->flags = O_WRONLY;
    desc2->pos = 0;
    desc2->inode = stderr;
    list_push_back(state->files, desc2);


    return state;
}


/**
 * kills a process
 * @param process pointer to the process state
 */
void process_kill(struct process_state *process)
{
    asm volatile("cli");
    send_killed_process(process);
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
        if(current_child->process)
            process_kill(current_child->process);
        free(current_child);
    }


    list_set_first(process->parent->children);
    while(!list_is_empty(process->parent->children))
    {
        struct child *current_child = list_get_current(process->parent->children);
        if(current_child->process == process)
        {
            current_child->process = 0;
            break;
        }
        list_next(process->parent->children);
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

/**
 * finds a process by ID
 * @param pid Process ID
 * @return process state pointer or NULL if the process does not exist
 */
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

/**
 * terminates the current process (linux function for the API)
 * @param cpu registers of the corrent process
 */
void sys_exit(struct cpu_state **cpu)
{
    list_set_first(current_thread->process->parent->children);
    while(list_is_last(current_thread->process->parent->children))
    {
        struct child *current_child = list_get_current(current_thread->process->parent->children);
        if(current_child->process == current_thread->process)
        {
            current_child->status = (*cpu)->CPU_ARG1;
            break;
        }
        list_next(current_thread->process->parent->children);
    }

    process_kill(current_thread->process);
    *cpu = task_schedule(*cpu);
}

/**
 * creates a new child process (linux function for the API)
 * @param cpu registers of the current process
 */
void sys_fork(struct cpu_state **cpu)
{
    vmm_context_t context;
    vmm_create_context(&context);
    arch_fork_context(&current_thread->context.arch_context, &context.arch_context);
    struct process_state *new_process = process_create(current_thread->process->name ,current_thread->process->desc ,current_thread->process->flags ,current_thread->process, NULL, NULL, NULL);
    struct thread_state *new_thread = thread_create(new_process, !(current_thread->flags & THREAD_KERNELMODE), 0, *cpu, 0, NULL, NULL, &context);

    void *stack_src = MEMORY_LAYOUT_STACK_TOP - THREAD_STACK_SIZE;
    paddr_t pframe = pmm_alloc_page();
    vmm_map(&new_thread->context, pframe, MEMORY_LAYOUT_STACK_TOP-0x1000, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
    void *stack = vmm_automap_kernel(current_context, pframe, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
    memcpy(stack, stack_src, THREAD_STACK_SIZE);

	struct list_node *node = current_thread->process->files->head->next;
	struct list_node *head = current_thread->process->files->head;
	while(node != head) {
		struct fd *dest = malloc(sizeof(struct fd));
		struct fd *src  = (struct fd*) node->element;
		memcpy(dest, src, sizeof(struct fd));
		list_push_back(new_process->files, dest);

		node = node->next;
	}

    new_thread->state->CPU_ARG0 = 0;
    current_thread->state->CPU_ARG0 = new_process->pid;
}

/**
 *  wait for the termination of a child (linux function for the API)
 *  @param cpu registers of the current process
 *  Not completed
 */

void sys_waitpid(struct cpu_state **cpu)
{
    list_set_first(running_threads);
    while(list_get_current(running_threads) != current_thread)
    {
        list_next(running_threads);
    }
    list_remove(running_threads);
    current_thread->ticks = 0;
    current_thread->flags |= THREAD_WAITPID;
    current_thread->waitpid = (*cpu)->CPU_ARG1;
    add_trigger(WAIT_PID, current_thread->waitpid, false, (void *)current_thread,NULL);
    *cpu = task_schedule(*cpu);
}



