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
struct process_state *process_create(const char *name, uint16_t flags,struct process_state *parent, uid_t uid, gid_t gid,struct pipeset *set)
{
    struct process_state *state = malloc(sizeof(struct process_state));

    // copy name string
    int string_len = min(strlen(name), 255);
    state->name = (char *) malloc(string_len + 1);
    strncpy(state->name, name, string_len);
    state->name[string_len + 1] = '\0';

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

    if(!(process->flags & PROCESS_ZOMBIE) )
    {
        free(process);
    }

    enable_irqs();
}


/**
 *  @brief Suspends a process
 *  @param 0 pointer to the process state
 *  @return void
 */
void process_suspend(struct process_state *object)
{
    if(list_is_empty(object->threads))
        return;

    iterator_t it = iterator_create(object->threads);
    list_set_first(&it);
    while(!list_is_last(&it))
    {
        struct thread_state *thread = (struct thread_state *)list_get_current(&it);
        if(thread->flags & THREAD_ACTIV)
            thread_suspend(thread);

        list_next(&it);
    }

    object->flags &= ~PROCESS_ACTIVE;
}

/**
 * Wakes up a process
 * @param 0 pointer to the process state
 * @return void
 **/
void process_wakeup(struct process_state *object)
{
    struct process_state *process = object;
    iterator_t it = iterator_create(process->threads);
    list_set_first(&it);
    while(!list_is_empty(process->threads) && !list_is_last(&it))
    {
        struct thread_state *thread = (struct thread_state *)list_get_current(&it);
        if(!(thread->flags & THREAD_ACTIV))
            thread_wakeup(thread);
        list_next(&it);
    }
    object->flags |= PROCESS_ACTIVE;
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


