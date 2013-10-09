#ifndef _process_h_
#define _process_h_
#include <paging.h>
#include <list.h>
#include <cpu.h>
#include <vfs.h>

typedef uint32_t tid_t;

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
    char *name;
    char *desc;
    struct process_state* parent;
    vfs_inode_t *cwd;
    list_t *files;
    list_t *ports;
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


    struct process_state *process_create(const char *name, const char *desc, uint16_t flags,struct process_state *parent);

    void process_kill(struct process_state *process);

    struct process_state *process_find(pid_t id);

    void sys_exit(struct cpu_state **cpu);

    void sys_fork(struct cpu_state **cpu);

#endif
