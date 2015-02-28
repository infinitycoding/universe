
#ifndef _FD_H
#define _FD_H

#include <sched/process.h>

typedef struct vfs_inode_descriptor
{
    vfs_inode_t *inode;
    int position;
} vfs_inode_descriptor_t;

typedef struct file_descriptor
{
    unsigned int id;
    mode_t mode;
    int flags;
    int permission;

    vfs_inode_descriptor_t *read_descriptor;
    vfs_inode_descriptor_t *write_descriptor;
} file_descriptor_t;

file_descriptor_t *get_fd(struct process_state *process, int fd);
file_descriptor_t *create_fd(struct process_state *process);

vfs_inode_descriptor_t *vfs_create_inode_descriptor(vfs_inode_t *inode);
size_t vfs_read_descriptor(vfs_inode_descriptor_t *desc, void *buffer, size_t length);
size_t vfs_write_descriptor(vfs_inode_descriptor_t *desc, void *buffer, size_t length);

#endif

