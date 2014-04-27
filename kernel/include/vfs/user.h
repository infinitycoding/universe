#ifndef _USER_H_
#define _USER_H_

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
 *  @author Peter Hösch aka. BlitzBasic <peter.hoesch@infinitycoding.de>
 */

#include <vfs/vfs.h>
#include <list.h>

#define SUCCESS 0
#define FAILTURE -1

#define YES 1
#define NO 0

typedef struct
{
	char *name;		// user name
	char *passwd;	// user password
	char *home;		// home directory
	uid_t id;		// user id
	list_t *grps;	// group ids
}user_t;

typedef struct
{
	char *name;		// group name
	gid_t id;		// group id
	list_t *users;	// users in group
}group_t;

void USER_INIT(void);
int add_user(const char *uname, const char *upasswd, const char *uhome, uid_t uid);
int remove_user(user_t *user);
user_t *get_user_by_id(uid_t id);
user_t *get_user_by_name(const char *uname);
int add_group(const char *groupname, gid_t gid);
int remove_group(group_t *group);
group_t *get_group_by_id(gid_t id);
group_t *get_group_by_name(const char *gname);
int add_user_to_group(user_t *user, group_t *group);
int remove_user_from_group(user_t *user, group_t *group);
int group_id_exists(gid_t id);
int user_id_exists(uid_t id); 
int group_name_exists(const char *name);
int user_name_exists(const char *name);


#endif