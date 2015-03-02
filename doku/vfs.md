The Virtual File System
=======

## Inodes
An inode represents an object in the VFS. It consists of:
* name & some meta infos (type, length)
* pointer to parent-inode
* buffers (block-buffer objects) to store data

#### Types
* regular
* pipe
* link

## Descriptors

<pre>
file descriptor:
        read/write descriptor -> inode_descriptor:
                                        inode -> Inode: 
                                                buffers[n] -> blockbuffer
                                        position, read_buffer_id, write_buffer_id
</pre>

#### Inode-Descriptor
Structure:
* pointer to inode
* position in data
* buffer id for read
* buffer id for write

Operations:

#### File-Descriptor
* userspace ID
* inode-descriptor for read
* inode-descriptor for write
