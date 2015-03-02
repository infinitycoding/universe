The Virtual File System
=======

Descriptors:
<pre>
file descriptor:
        read/write descriptor -> inode_descriptor:
                                        inode -> Inode: 
                                                buffers[n] -> blockbuffer
                                        position, read_buffer_id, write_buffer_id
</pre>
