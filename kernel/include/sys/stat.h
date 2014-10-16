#ifndef _STAT_H_
#define _STAT_H_

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
	@author Michael Sippel <michamimosa@gmail.com>
*/

#include <stdint.h>
#include <sys/unistd.h>
#include <sys/types.h>

#define S_IFMT  0x0 // type of file
#define S_IFBLK 0x1 // block special
#define S_IFCHR 0x2 // character special
#define S_IFIFO 0x3 // FIFO special
#define S_IFREG 0x4 // regular
#define S_IFDIR 0x5 // directory
#define S_IFLNK 0x6 // symbolic link

#define S_MAO 0x6 // st_mode: access offset

#define S_IRUSR (0x001 << S_MAO) //user-read
#define S_IWUSR (0x002 << S_MAO) // -write
#define S_IXUSR (0x004 << S_MAO) // -execute
#define S_IRGRP (0x008 << S_MAO) //group-read
#define S_IWGRP (0x010 << S_MAO) // -write
#define S_IXGRP (0x020 << S_MAO) // -execute
#define S_IROTH (0x040 << S_MAO) //other-read
#define S_IWOTH (0x080 << S_MAO) // -write
#define S_IXOTH (0x100 << S_MAO) // -execute

// check
#define S_ISBLK(x) (x.st_mode & S_IFBLK) ? 1 : 0
#define S_ISCHR(x) (x.st_mode & S_IFCHR) ? 1 : 0
#define S_ISDIR(x) (x.st_mode & S_IFDIR) ? 1 : 0
#define S_ISFIFO(x) (x.st_mode & S_IFIFO) ? 1 : 0
#define S_ISREG(x) (x.st_mode & S_IFREG) ? 1 : 0
#define S_ISLNK(x) (x.st_mode & S_IFLNK) ? 1 : 0

struct stat
{
    mode_t 	st_mode; // access rights
    ino_t	st_ino;
    //       dev_t	st_dev; // filesystem
    //       dev_t	st_rdev;

    nlink_t	st_nlink;
    uid_t	st_uid;
    gid_t	st_gid;
    off_t	st_size;

    time_t	st_atime; // access time
    time_t	st_mtime; // modification time
    time_t	st_ctime; // create time

    long	st_blksize;
    long	st_blocks;
};

#endif
