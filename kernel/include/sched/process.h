#ifndef _PROCESS_H_
#define _PROCESS_H_

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

#include <mm/paging.h>
#include <vfs/vfs.h>
#include <list.h>
#include <cpu.h>


typedef uint32_t tid_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;

struct pipeset
{
    vfs_inode_t *stdin;
    vfs_inode_t *stdout;
    vfs_inode_t *stderr;
};

typedef enum
{
    PROCESS_ACTIVE = 1,
    PROCESS_FREEZED =  2,
    PROCESS_ZOMBIE = 4,
    PROCESS_NOT_FOUND = -1,
}pflag_t;

typedef uint32_t pid_t;

typedef enum
{
    KERNELMODE = 0,
    USERMODE = 3
} privilege_t;

struct process_state
{
    pid_t pid;
    uid_t uid;
    gid_t gid;
    char *name;
    char *desc;
    struct process_state* parent;
    vfs_inode_t *cwd;
    list_t *files;
    list_t *ports;
    list_t *env;
    list_t *threads;
    tid_t tid_counter;
    struct thread_state *main_thread;
    list_t *zombie_tids;
    list_t *children;
    uint16_t flags;
};

struct child
{
    int status;
    struct process_state *process;
};

    struct process_state *process_create(const char *name, const char *desc, uint16_t flags,struct process_state *parent, uid_t uid, gid_t gid,struct pipeset *set);
    void process_kill(struct process_state *process);
    struct process_state *process_find(pid_t id);
    void sys_exit(struct cpu_state **cpu);
    void sys_fork(struct cpu_state **cpu);
    void sys_waitpid(struct cpu_state **cpu);
    void sys_getpid(struct cpu_state **cpu);
    void dump_thread_list(list_t *threads);

#endif
