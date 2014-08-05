
#ifndef _FD_H
#define _FD_H

#include <sched/process.h>

struct fd
{
    unsigned int id;
    mode_t mode;
    int flags;
    int permission;

    vfs_inode_t *read_inode;
    vfs_inode_t *write_inode;
    int read_pos;
    int write_pos;
};

struct fd *get_fd(struct process_state *process, int fd);
struct fd *create_fd(struct process_state *process);

#endif

