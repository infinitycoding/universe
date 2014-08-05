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
#include <sys/errorcode.h>
#include <sched/process.h>
#include <sched/thread.h>
#include <sched/elf.h>
#include <mm/heap.h>
#include <string.h>
#include <sched/scheduler.h>
#include <mm/paging.h>
#include <memory_layout.h>
#include <event/trigger.h>
#include <printf.h>
#include <math.h>
#include <vfs/fd.h>


extern struct process_state *kernel_state;
extern struct thread_state *current_thread;
extern list_t *running_threads;
extern iterator_t thread_iterator;
extern vfs_inode_t *root;


list_t *process_list = 0;
list_t *zombie_list = 0;
pid_t pid_counter = 1;


/**
 * @breif Prints a thread-list.
 * @param The thread list
 */
void dump_thread_list(list_t *threads)
{
    iterator_t it = iterator_create(threads);
    printf("-----%d-----\n", list_length(threads));
    while(!list_is_empty(threads) && !list_is_last(&it))
    {
        struct thread_state *t = list_get_current(&it);
        printf("PID: %d  TID: %d  FLAG: %08x\n",t->process->pid, t->tid, t->flags);
        list_next(&it);
    }
}


/**
 * @brief creates a process
 * @param name      name of the process (max. 255 characters)
 * @param desc      description of the process (max. 255 characters)
 * @param flags     process flags (activ, freezed, zombies)
 * @param parent    pointer to the parent process struct (NULL: Kernel Init = parent)
 * @return          The new process-state
 */
struct process_state *process_create(const char *name, const char *desc, uint16_t flags,struct process_state *parent, uid_t uid, gid_t gid,struct pipeset *set)
{
    struct process_state *state = malloc(sizeof(struct process_state));

    // copy name string
    int string_len = min(strlen(name), 255);
    state->name = (char *) malloc(string_len + 1);
    strncpy(state->name, name, string_len);
    state->name[string_len + 1] = '\0';

    //copy description string
    string_len = min(strlen(desc), 255);
    state->desc = (char *) malloc(string_len + 1);
    strncpy(state->desc, desc, string_len);
    state->desc[string_len + 1] = '\0';

    state->flags = flags;
    state->files = list_create();
    state->heap_top = 0;
    state->heap_lower_limit = 0;
    state->heap_upper_limit = 0;

    state->main_thread = NULL;

    // if parent is not set the working directory is the root dir
    if(parent != NULL)
        state->cwd = parent->cwd;
    else
        state->cwd = root;

    state->children = list_create();
    state->zombie_tids = list_create();
    state->threads = list_create();
    state->ports = list_create();
    state->tid_counter = 1;
    state->uid = uid;
    state->gid = gid;

    if (parent == NULL)
        state->parent = kernel_state;
    else
        state->parent = parent;

    if (list_is_empty(zombie_list))
        state->pid = pid_counter++;
    else
        state->pid = (pid_t) list_pop_front(zombie_list);


    list_lock(process_list);
    if(state->pid != 1)
    {
        struct child *new_child = malloc(sizeof(struct child));
        new_child->process = state;
        new_child->status = 0;
        list_push_front(state->parent->children, new_child);
    }

    list_push_front(process_list, state);
    list_unlock(process_list);


    /* create stream files*/
    vfs_inode_t *stdin;
    vfs_inode_t *stdout;
    vfs_inode_t *stderr;
    if(!set)
    {
        stdin = vfs_create_pipe(uid, gid);
        stdout = vfs_create_pipe(uid, gid);
        stderr = vfs_create_pipe(uid, gid);
    }
    else
    {
        stdin = set->stdin;
        stdout = set->stdout;
        stderr = set->stderr;
    }


    struct fd *desc0 = malloc(sizeof(struct fd));
    desc0->id = 0;
    desc0->mode = 0x7ff;
    desc0->flags = O_RDONLY;
    desc0->permission = VFS_PERMISSION_READ;
    desc0->read_pos = 0;
    desc0->write_pos = 0;
    desc0->read_inode = stdin;
    desc0->write_inode = NULL;
    list_push_back(state->files, desc0);

    struct fd *desc1 = malloc(sizeof(struct fd));
    desc1->id = 1;
    desc1->mode = 0x7ff;
    desc1->flags = O_WRONLY;
    desc1->permission = VFS_PERMISSION_WRITE;
    desc1->read_pos = 0;
    desc1->write_pos = 0;
    desc1->read_inode = NULL;
    desc1->write_inode = stdout;
    list_push_back(state->files, desc1);

    struct fd *desc2 = malloc(sizeof(struct fd));
    desc2->id = 2;
    desc2->mode = 0x7ff;
    desc2->flags = O_WRONLY;
    desc2->permission = VFS_PERMISSION_WRITE;
    desc2->read_pos = 0;
    desc2->write_pos = 0;
    desc2->read_inode = NULL;
    desc2->write_inode = stderr;
    list_push_back(state->files, desc2);


    return state;
}


/**
 * @brief kills a process
 * @param process pointer to the process state
 */
void process_kill(struct process_state *process)
{
    disable_irqs();

    send_killed_process(process);

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

    list_lock(process->children);
    while(!list_is_empty(process->children))
    {
        struct child *current_child = list_pop_front(process->children);
        if(current_child->process)
            process_kill(current_child->process);
        free(current_child);
    }
    free(process->children);

    list_lock(process->parent->children);
    iterator_t parents_children_it = iterator_create(process->parent->children);

    while(!list_is_empty(process->parent->children) && !list_is_last(&parents_children_it))
    {
        struct child *current_child = list_get_current(&parents_children_it);
        if(current_child->process == process)
        {
            current_child->process = 0;
            break;
        }
        list_next(&parents_children_it);
    }

    list_unlock(process->parent->children);
    list_destroy(process->ports);
    list_destroy(process->zombie_tids);
    list_lock(process_list);

    iterator_t process_it = iterator_create(process_list);

    while(!list_is_empty(process_list) &&!list_is_last(&process_it))
    {
        if(((struct process_state *)list_get_current(&process_it)) == process)
        {
            list_remove(&process_it);
            break;
        }
        list_next(&process_it);
    }
    list_unlock(process_list);

    free(process->name);
    free(process->desc);

    if(!(process->flags & PROCESS_ZOMBIE) )
    {
        free(process);
    }

    enable_irqs();
}

/**
 * @brief finds a process by ID
 * @param pid Process ID
 * @return process state pointer or NULL if the process does not exist
 */
struct process_state *process_find(pid_t pid)
{
    iterator_t process_it = iterator_create(process_list);
    while(!list_is_empty(process_list) && !list_is_last(&process_it))
    {
        if(((struct process_state *)list_get_current(&process_it))->pid == pid)
        {
            return (struct process_state *)list_get_current(&process_it);
        }
        list_next(&process_it);
    }
    return 0;
}

/**
 * @brief terminates the current process (linux function for the API)
 * @param cpu registers of the corrent process
 */
void sys_exit(struct cpu_state **cpu)
{
    iterator_t parents_children_it = iterator_create(current_thread->process->parent->children);

    while(!list_is_empty(current_thread->process->parent->children) && !list_is_last(&parents_children_it))
    {
        struct child *current_child = list_get_current(&parents_children_it);
        if(current_child->process == current_thread->process)
        {
            current_child->status = (*cpu)->CPU_ARG1;
            break;
        }
        list_next(&parents_children_it);
    }

    process_kill(current_thread->process);
    *cpu = task_schedule(*cpu);
}

/**
 * @brief creates a new child process (linux function for the API)
 * @param cpu registers of the current process
 */
void sys_fork(struct cpu_state **cpu)
{
    vmm_context_t context;
    vmm_create_context(&context);
    arch_fork_context(&current_thread->context.memory.arch_context, &context.arch_context);
    struct process_state *new_process = process_create(current_thread->process->name ,current_thread->process->desc ,current_thread->process->flags ,current_thread->process, current_thread->process->uid, current_thread->process->gid, NULL);
    struct thread_state *new_thread = thread_clone(new_process, current_thread);

    void *stack_src = (void *)(MEMORY_LAYOUT_STACK_TOP - THREAD_STACK_SIZE);
    paddr_t pframe = pmm_alloc_page();
    vmm_map(&new_thread->context.memory, pframe, MEMORY_LAYOUT_STACK_TOP-0x1000, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
    void *stack = (void *)vmm_automap_kernel(current_context, pframe, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
    memcpy(stack, stack_src, THREAD_STACK_SIZE);

    struct list_node *node = current_thread->process->files->head->next;
    struct list_node *head = current_thread->process->files->head;
    while(node != head)
    {
        struct fd *dest = malloc(sizeof(struct fd));
        struct fd *src  = (struct fd*) node->element;
        memcpy(dest, src, sizeof(struct fd));
        list_push_back(new_process->files, dest);

        node = node->next;
    }

    new_thread->context.state->CPU_ARG0 = 0;
    current_thread->context.state->CPU_ARG0 = new_process->pid;
}

/**
 *  @brief wait for the termination of a child (linux function for the API)
 *  @param cpu registers of the current process
 *  Not completed
 */

void sys_waitpid(struct cpu_state **cpu)
{
    list_set_first(&thread_iterator);
    while(list_get_current(&thread_iterator) != current_thread)
    {
        list_next(&thread_iterator);
    }
    list_remove(&thread_iterator);
    current_thread->ticks = 0;
    current_thread->flags |= THREAD_WAITPID;
    current_thread->waitpid = (*cpu)->CPU_ARG1;
    add_trigger(WAIT_PID, current_thread->waitpid, false, (void *)current_thread,NULL);
    *cpu = task_schedule(*cpu);
}

void sys_getpid(struct cpu_state **cpu)
{
    (*cpu)->CPU_ARG0 = current_thread->process->pid;
}


/**
 * executes a programm
 * @param cpu registers of the current process
 * todo: the function is still a litte bit slow and envp is not taken over from the new process.
 */
void sys_execve(struct cpu_state **cpu)
{
    char *filename = (char*) (*cpu)->CPU_ARG1;
    char **argv = (char**) (*cpu)->CPU_ARG2;
    //char **envp = (char**) (*cpu)->CPU_ARG3;

    vfs_inode_t *filenode = vfs_lookup_path(filename);
    printf(filename);
    if(filenode == NULL)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }

    struct process_state *process = current_thread->process;

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

    list_destroy(process->ports);
    list_destroy(process->zombie_tids);
    process->zombie_tids = list_create();

    // run the new process
    load_elf_thread_from_file(filenode, process, 0,(void**) argv);
}
