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
#include <sched/thread.h>
#include <vfs/user.h>
#include <mm/heap.h>
#include <string.h>
#include <cpu.h>


extern struct thread_state *current_thread;

list_t *users;		// user list
list_t *groups;		// group list

void USER_INIT(void)
{
    users = list_create();
    groups = list_create();

    add_group("root", 0);
    add_user("root","","/",0);
    add_user_to_group(get_user_by_id(0), get_group_by_id(0));
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
    list_lock(users);
    list_push_back(users, new_user);
    list_unlock(users);

    return SUCCESS;
}

int remove_user(user_t *user)
{
    iterator_t user_itr = iterator_create(users);
    list_lock(users);
    while(!list_is_last(&user_itr))
    {
        if(((user_t *)list_get_current(&user_itr)) == user)
        {
            user_t *usr = ((user_t *)list_get_current(&user_itr));

            iterator_t grps_itr = iterator_create(usr->grps);
            list_lock(usr->grps);
            while(!list_is_last(&grps_itr))
            {
                remove_user_from_group_lockfree(usr, (group_t *)list_get_current(&grps_itr));

                list_next(&grps_itr);
            }
            list_unlock(usr->grps);

            free(usr->name);
            free(usr->passwd);
            free(usr->home);
            list_destroy(usr->grps);
            free(usr);

            list_remove(&user_itr);
            list_unlock(users);
            return SUCCESS;
        }

        list_next(&user_itr);
    }

    list_unlock(users);
    return FAILTURE;
}

user_t *get_user_by_id(uid_t id)
{
    iterator_t user_itr = iterator_create(users);
    list_lock(users);
    while(!list_is_last(&user_itr))
    {
        if(((user_t *)list_get_current(&user_itr))->id == id)
        {
            user_t *target = (user_t *)list_get_current(&user_itr);
            list_unlock(users);
            return target;
        }

        list_next(&user_itr);
    }
    list_unlock(users);
    return NULL;
}

user_t *get_user_by_name(const char *uname)
{
    iterator_t user_itr = iterator_create(users);
    list_lock(users);

    while(!list_is_last(&user_itr))
    {
        if(strcmp(((user_t *)list_get_current(&user_itr))->name, uname) == 0)
        {
            user_t *target = (user_t *)list_get_current(&user_itr);
            list_unlock(users);
            return target;
        }

        list_next(&user_itr);
    }

    list_unlock(users);
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
    list_lock(groups);
    list_push_back(groups, new_group);
    list_unlock(groups);

    return SUCCESS;
}

int remove_group(group_t *group)
{
    iterator_t grps_itr = iterator_create(groups);
    list_lock(groups);
    while(!list_is_last(&grps_itr))
    {
        if(((group_t *)list_get_current(&grps_itr)) == group)
        {
            group_t *grp = ((group_t *)list_get_current(&grps_itr));

            iterator_t user_itr = iterator_create(grp->users);
            list_lock(grp->users);
            while(!list_is_last(&user_itr))
            {
                remove_user_from_group_lockfree((user_t *)list_get_current(&user_itr), grp);

                list_next(&user_itr);
            }
            list_unlock(grp->users);

            free(grp->name);
            list_destroy(grp->users);
            free(grp);

            list_remove(&grps_itr);
            list_unlock(groups);
            return SUCCESS;
        }

        list_next(&grps_itr);
    }
    list_unlock(groups);
    return FAILTURE;
}

group_t *get_group_by_id(gid_t id)
{
    iterator_t grps_itr = iterator_create(groups);
    list_lock(groups);

    while(!list_is_last(&grps_itr))
    {
        if(((group_t *)list_get_current(&grps_itr))->id == id)
        {
            list_unlock(groups);
            return ((group_t *)list_get_current(&grps_itr));
        }

        list_next(&grps_itr);
    }

    list_unlock(groups);

    return NULL;
}

group_t *get_group_by_name(const char *gname)
{
    iterator_t grps_itr = iterator_create(groups);
    list_lock(groups);

    while(!list_is_last(&grps_itr))
    {
        if(strcmp(((group_t *)list_get_current(&grps_itr))->name, gname) == 0)
        {
            list_unlock(groups);
            return ((group_t *)list_get_current(&grps_itr));
        }

        list_next(&grps_itr);
    }

    list_unlock(groups);

    return NULL;
}

int add_user_to_group(user_t *user, group_t *group)
{
    if(user == NULL || group == NULL)
        return FAILTURE;

    list_lock(user->grps);
    list_push_back(user->grps, group);
    list_unlock(user->grps);

    list_lock(group->users);
    list_push_back(group->users, user);
    list_unlock(group->users);

    return SUCCESS;
}

int remove_user_from_group(user_t *user, group_t *group)
{
    list_lock(group->users);
    int result = remove_user_from_group_lockfree(user, group);
    list_unlock(group->users);

    return result;
}

int remove_user_from_group_lockfree(user_t *user, group_t *group)
{
    iterator_t grps_itr = iterator_create(user->grps);

    while(!list_is_last(&grps_itr))
    {
        if((group_t *)list_get_current(&grps_itr) == group)
        {
            list_remove(&grps_itr);
            break;
        }

        list_next(&grps_itr);
    }

    iterator_t user_itr = iterator_create(group->users);

    while(!list_is_last(&user_itr))
    {
        if((user_t *)list_get_current(&user_itr) == user)
        {
            list_remove(&user_itr);
            return SUCCESS;
        }

        list_next(&user_itr);
    }

    return FAILTURE;
}

int group_id_exists(gid_t id)
{
    iterator_t grps_itr = iterator_create(groups);
    list_lock(groups);

    while(!list_is_last(&grps_itr))
    {
        if(((group_t *)list_get_current(&grps_itr))->id == id)
        {
            list_unlock(groups);
            return YES;
        }

        list_next(&grps_itr);
    }

    list_unlock(groups);

    return NO;
}

int user_id_exists(uid_t id)
{
    iterator_t user_itr = iterator_create(users);
    list_lock(users);

    while(!list_is_last(&user_itr))
    {
        if(((user_t *)list_get_current(&user_itr))->id == id)
        {
            list_unlock(users);
            return YES;
        }

        list_next(&user_itr);
    }

    list_unlock(users);

    return NO;
}

int group_name_exists(const char *name)
{
    iterator_t grps_itr = iterator_create(groups);
    list_lock(groups);

    while(!list_is_last(&grps_itr))
    {
        if(strcmp(((group_t *)list_get_current(&grps_itr))->name, name) == 0)
        {
            list_unlock(groups);
            return YES;
        }

        list_next(&grps_itr);
    }

    list_unlock(groups);

    return NO;
}

int user_name_exists(const char *name)
{
    iterator_t user_itr = iterator_create(users);
    list_lock(users);

    while(!list_is_last(&user_itr))
    {
        if(strcmp(((user_t *)list_get_current(&user_itr))->name, name) == 0)
        {
            list_unlock(users);
            return YES;
        }

        list_next(&user_itr);
    }

    list_unlock(users);

    return NO;
}


void sys_getuid(struct cpu_state **cpu)
{
    (*cpu)->CPU_ARG0 = current_thread->process->uid;
}

void sys_setuid(struct cpu_state **cpu)
{
    if(user_id_exists((*cpu)->CPU_ARG1))
    {
        (*cpu)->CPU_ARG0 = -1;
        return;
    }

    user_t *user = get_user_by_id(current_thread->process->uid);
    user->id = (*cpu)->CPU_ARG1;
    //todo: change the uids of each process and file in the home durectory
    (*cpu)->CPU_ARG0 = 1;
}