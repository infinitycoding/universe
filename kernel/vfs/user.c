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
 **/

#include <vfs/user.h>
#include <mm/heap.h>
#include <string.h>


list_t *users;		// user list
list_t *groups;		// group list

void USER_INIT(void)
{
	users = list_create();
	groups = list_create();

	add_group("kernel", 0);
}

int add_user(const char *uname, const char *upasswd, const char *uhome, uid_t uid)
{
	if(uname == NULL || uname[0] == '\0')
		return FAILTURE;

	if(user_id_exists(uid))
		return FAILTURE;

	if(user_name_exists(uname))
		return FAILTURE;

	user_t *new_user = (user_t *)malloc(sizeof(user_t));

	new_user->name = (char *)malloc(sizeof(char) * strlen(uname));
	strcpy(new_user->name, uname);

	if(upasswd == NULL)
		new_user->passwd = NULL;
	else
	{
		new_user->passwd = (char *)malloc(sizeof(char) * strlen(upasswd));
		strcpy(new_user->passwd, upasswd);
	}

	if(uhome != NULL)
	{
		new_user->home = (char *)malloc(sizeof(char) * strlen(uhome));
		strcpy(new_user->home, uhome);
	}
	else
	{
		new_user->home = (char *)malloc(sizeof(char) * (strlen("/home/") + strlen(uname)));
		strcpy(new_user->home, "/home/");
		strcat(new_user->home, uname);
		vfs_create_inode(new_user->name, S_IRUSR | S_IWUSR, vfs_lookup_path("/home/"), uid, 0);
	}

	new_user->id = uid;
	new_user->grps = list_create();
	list_push_back(users, new_user);

	return SUCCESS;
}

int remove_user(user_t *user)
{
	list_set_first(users);

	while(!list_is_last(users))
	{
		if(((user_t *)list_get_current(users)) == user)
		{
			user_t *usr = ((user_t *)list_get_current(users));

			list_set_first(usr->grps);

			while(!list_is_last(usr->grps))
			{
				remove_user_from_group(usr, (group_t *)list_get_current(usr->grps));

				list_next(usr->grps);
			}

			free(usr->name);
			free(usr->passwd);
			free(usr->home);
			list_destroy(&usr->grps);
			free(usr);

			list_remove(users);
			return SUCCESS;
		}

		list_next(users);
	}

	return FAILTURE;
}

user_t *get_user_by_id(uid_t id)
{
	list_set_first(users);

	while(!list_is_last(users))
	{
		if(((user_t *)list_get_current(users))->id == id)
			return ((user_t *)list_get_current(users));

		list_next(users);
	}

	return NULL;
}

user_t *get_user_by_name(const char *uname)
{
	list_set_first(users);

	while(!list_is_last(users))
	{
		if(strcmp(((user_t *)list_get_current(users))->name, uname) == 0)
			return ((user_t *)list_get_current(users));

		list_next(users);
	}

	return NULL;
}

int add_group(const char *groupname, gid_t gid)
{
	if(groupname == NULL || groupname[0] == '\0')
		return FAILTURE;

	if(group_id_exists(gid))
		return FAILTURE;

	if(group_name_exists(groupname))
		return FAILTURE;

	group_t *new_group = (group_t *)malloc(sizeof(group_t));

	new_group->name = (char *)malloc(sizeof(char) * strlen(groupname));
	strcpy(new_group->name, groupname);

	new_group->id = gid;
	new_group->users = list_create();
	list_push_back(groups, new_group);

	return SUCCESS;
}

int remove_group(group_t *group)
{
	list_set_first(groups);

	while(!list_is_last(groups))
	{
		if(((group_t *)list_get_current(groups)) == group)
		{
			group_t *grp = ((group_t *)list_get_current(groups));

			list_set_first(grp->users);

			while(!list_is_last(grp->users))
			{
				remove_user_from_group((user_t *)list_get_current(grp->users), grp);

				list_next(grp->users);
			}

			free(grp->name);
			list_destroy(&grp->users);
			free(grp);

			list_remove(groups);
			return SUCCESS;
		}

		list_next(groups);
	}

	return FAILTURE;
}

group_t *get_group_by_id(gid_t id)
{
	list_set_first(groups);

	while(!list_is_last(groups))
	{
		if(((group_t *)list_get_current(groups))->id == id)
			return ((group_t *)list_get_current(groups));

		list_next(groups);
	}

	return NULL;
}

group_t *get_group_by_name(const char *gname)
{
	list_set_first(groups);

	while(!list_is_last(groups))
	{
		if(strcmp(((group_t *)list_get_current(groups))->name, gname) == 0)
			return ((group_t *)list_get_current(groups));

		list_next(groups);
	}

	return NULL;
}

int add_user_to_group(user_t *user, group_t *group)
{
	if(user == NULL || group == NULL)
		return FAILTURE;

	list_push_back(user->grps, group);
	list_push_back(group->users, user);

	return SUCCESS;
}

int remove_user_from_group(user_t *user, group_t *group)
{
	list_set_first(user->grps);

	while(!list_is_last(user->grps))
	{
		if((group_t *)list_get_current(user->grps) == group)
		{
			list_remove(user->grps);
			break;
		}

		list_next(user->grps);
	}

	list_set_first(group->users);

	while(!list_is_last(group->users))
	{
		if((user_t *)list_get_current(group->users) == user)
		{
			list_remove(group->users);
			return SUCCESS;
		}

		list_next(group->users);
	}

	return FAILTURE;
}

int group_id_exists(gid_t id)
{
	list_set_first(groups);

	while(!list_is_last(groups))
	{
		if(((group_t *)list_get_current(groups))->id == id)
			return YES;

		list_next(groups);
	}

	return NO;
}

int user_id_exists(uid_t id)
{
	list_set_first(users);

	while(!list_is_last(users))
	{
		if(((user_t *)list_get_current(users))->id == id)
			return YES;

		list_next(users);
	}

	return NO;
}

int group_name_exists(const char *name)
{
	list_set_first(groups);

	while(!list_is_last(groups))
	{
		if(strcmp(((group_t *)list_get_current(groups))->name, name) == 0)
			return YES;

		list_next(groups);
	}

	return NO;
}

int user_name_exists(const char *name)
{
	list_set_first(users);

	while(!list_is_last(users))
	{
		if(strcmp(((user_t *)list_get_current(users))->name, name) == 0)
			return YES;

		list_next(users);
	}

	return NO;
}