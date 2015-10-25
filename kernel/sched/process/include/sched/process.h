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
 *
 *  reviewed by Micha at 22.10.2014
 */

#include <mm/vmm.h>
#include <list.h>
#include <cpu.h>


typedef uint32_t tid_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;

typedef enum
{
    PROCESS_ACTIVE = 1,
    PROCESS_FREEZED =  2,
    PROCESS_ZOMBIE = 4,
    PROCESS_NOT_FOUND = -1,
} pflag_t;

typedef uint32_t pid_t;

struct env_var
{
    char *name;
    char *var;
};

struct process_state
{
    // identifiers
    pid_t pid;
    uid_t uid;
    gid_t gid;
    char *name;

    // parent
    struct process_state *parent;

    // files & ports
    list_t *ports;

    // threads
    struct thread_state *main_thread;
    list_t *threads;
    list_t *zombie_tids;
    tid_t tid_counter;

    // child processes
    list_t *children;

    // flags
    uint16_t flags;

    // memory stuff
    uint32_t heap_top;
    uint32_t heap_lower_limit;
    uint32_t heap_upper_limit;

    // shm
    list_t *shm_descriptors;

    // signals
    list_t *signals;
};

struct child
{
    int status;
    struct process_state *process;
};

enum signal_action
{
    NONE,
    WAKEUP,
    HANDLER,
    THREAD,
};

typedef struct signal
{
    unsigned int id;
    enum signal_action action;
    void (*handler)(int pid);
} signal_t;


void dump_thread_list(list_t *threads);

struct process_state *process_create(const char *name, uint16_t flags, struct process_state *parent, uid_t uid, gid_t gid);
struct process_state *process_find(pid_t id);

void process_kill(struct process_state *process);
void process_suspend(struct process_state *process);
void process_wakeup(struct process_state *process);

#endif

