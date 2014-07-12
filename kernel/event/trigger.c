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
 */


#include <event/trigger.h>
#include <list.h>
#include <mm/heap.h>
#include <sched/scheduler.h>
#include <idt.h>

extern struct thread_state *current_thread;
extern list_t *process_list;
extern list_t *running_threads;

list_t *trigger_list = 0;
list_t *event_id_list = 0;
uint32_t event_id_counter = 1;


/**
 * Initiates the process/thread trigger module
 * @param void
 * @return void
 **/
void INIT_TRIGGER(void)
{
    trigger_list = list_create();
    event_id_list = list_create();
}


/**
 * Suspends a running thread
 * @param 0 pointer to the thread state
 * @return void
 */
void suspend_thread(struct thread_state *object)
{
    if(list_is_empty(running_threads))
        return;

    iterator_t it = iterator_create(running_threads);
    list_set_first(&it);
    while(list_get_current(&it) != object && !list_is_last(&it))
        list_next(&it);

    if(list_get_current(&it) == object)
    {
        list_remove(&it);
        object->ticks  =  0;
        object->flags &= ~THREAD_ACTIV;
    }
}


/**
 * Suspends a process
 * @param 0 pointer to the process state
 * @return void
 */
void suspend_process(struct process_state *object)
{
    if(list_is_empty(object->threads))
        return;

    iterator_t it = iterator_create(object->threads);
    list_set_first(&it);
    while(!list_is_last(&it))
    {
        struct thread_state *thread = (struct thread_state *)list_get_current(&it);
        if(thread->flags & THREAD_ACTIV)
            suspend_thread(thread);

        list_next(&it);
    }

    object->flags &= ~PROCESS_ACTIVE;
}


/**
 * Wakes up a thread
 * @param 0 pointer to the thread state
 * @return void
 **/
void wakeup_thread(struct thread_state *object)
{
    list_push_front(running_threads, object);
    object->flags |= THREAD_ACTIV;
}


/**
 * Wakes up a process
 * @param 0 pointer to the process state
 * @return void
 **/
void wakeup_process(struct process_state *object)
{
    struct process_state *process = object;
    iterator_t it = iterator_create(process->threads);
    list_set_first(&it);
    while(!list_is_empty(process->threads) && !list_is_last(&it))
    {
        struct thread_state *thread = (struct thread_state *)list_get_current(&it);
        if(!(thread->flags & THREAD_ACTIV))
            wakeup_thread(thread);
        list_next(&it);
    }
    object->flags |= PROCESS_ACTIVE;
}


/**
 * Removes a event trigger by object name and ID (if given),
 * @param 0 pointer to thread/process state
 * @param 1 optional event ID (0 = don't care)
 * @return number or removed triggers
 **/
int remove_event_trigger(void *object, uint32_t ID)
{
    int removed_elements = 0;
    iterator_t it = iterator_create(trigger_list);
    list_set_first(&it);
    if(ID == 0)
    {
        while(!list_is_last(&it) && !list_is_empty(trigger_list))
        {
            struct trigger_entry *current_entry = list_get_current(&it);
            if(current_entry->object == object)
            {
                list_remove(&it);
                if(current_entry->type == WAIT_EVENT)
                    list_push_front(event_id_list,(void*)current_entry->ID);
                free(current_entry);
                removed_elements++;
            }
            list_next(&it);
        }
    }
    else
    {
        while(!list_is_last(&it) && !list_is_empty(trigger_list))
        {
            struct trigger_entry *current_entry = list_get_current(&it);
            if(current_entry->object == object && current_entry->ID == ID)
            {
                list_remove(&it);
                if(current_entry->type == WAIT_EVENT)
                    list_push_front(event_id_list,(void*)current_entry->ID);
                free(current_entry);
                removed_elements++;
                break;
            }
            list_next(&it);
        }
    }
    return removed_elements;
}


/**
 * Removes every trigger with the given ID
 * @param 0 event ID
 * @return number of removed triggers, 0 if there was no trigger with the given ID
 **/
int remove_event(uint32_t ID)
{
    int removed_elements = 0;
    iterator_t it = iterator_create(trigger_list);
    list_set_first(&it);
    while(!list_is_last(&it) && !list_is_empty(trigger_list))
    {
        struct trigger_entry *current_entry = list_get_current(&it);
        if(current_entry->ID == ID)
        {
            list_remove(&it);
            if(current_entry->type == WAIT_EVENT)
                list_push_front(event_id_list,(void*)current_entry->ID);
            free(current_entry);
            removed_elements++;
            break;
        }

        list_next(&it);
    }
    return removed_elements;
}


/**
 * Send a event which can lauch a event trigger
 * @param  0 event ID
 * @return true if the trigger was pulled, false if there's no trigger with the given ID
 */
int send_event(uint32_t ID)
{
    int ret = false;
    iterator_t it = iterator_create(trigger_list);
    while(!list_is_last(&it) && !list_is_empty(trigger_list))
    {
        struct trigger_entry *current_entry = list_get_current(&it);
        if(current_entry->ID == ID && current_entry->type == WAIT_EVENT)
        {
            if(current_entry->proc)
            {
                wakeup_process(current_entry->object);
                remove_event_trigger(current_entry->object, current_entry->ID);
            }
            else
            {
                struct thread_state *thread = current_entry->object;
                remove_event_trigger(current_entry->object, current_entry->ID);
                wakeup_thread(thread);

                if(current_entry->callback != NULL)
                {
                    // save current thread
                    struct thread_state *tmp = current_thread;

                    struct cpu_state **cpu = &thread->context.state;
                    current_thread = thread;
                    vmm_switch_context(&current_thread->context.memory);

                    current_entry->callback(cpu);

                    current_thread = tmp;
                    vmm_switch_context(&current_thread->context.memory);
                }
            }
            // that's a temporary workaround. I have to fix it later.
            // the problems are caused by blitzbasics list ports.
            ret = true;
            list_remove(&it);
            if(current_entry->type == WAIT_EVENT)
                list_push_front(event_id_list,(void*)current_entry->ID);
            free(current_entry);
            if(list_is_last(&it))
                return ret;
            list_set_first(&it);
        }
        else
            list_next(&it);
    }
    return ret;
}


/**
 * returns a new, unused event ID
 * @param void
 * @return new event ID
 */
uint32_t get_new_event_ID(void)
{
//    if(list_is_empty(event_id_list))
    return event_id_counter++;

    return (uint32_t) list_pop_back(event_id_list);
}


/**
 * Adds a event trigger
 * @param 0 true = object is a process, false = object is a thread
 * @param 1 pointer to process/thread state
 * @param 2 pointer to callback function (will be called before the thread/process wakes up) or NULL if there is no need for a callback
 * @return event ID
 **/
uint32_t add_event_trigger(bool proc, void *object, void (*callback)(struct cpu_state **cpu))
{
    struct trigger_entry *new_entry = (struct trigger_entry*) malloc(sizeof(struct trigger_entry));
    new_entry->type = WAIT_EVENT;
    new_entry->ID = get_new_event_ID();
    new_entry->proc = proc;
    new_entry->object = object;
    new_entry->callback = callback;

    list_push_front(trigger_list,new_entry);
    return new_entry->ID;
}


/**
 * Adds a trigger
 * @param 0 trigger type
 * @param 1 trigger ID (if type is not EVENT, the ID is unimportant and can be uesd as custom Identifier)
 * @param 2 true = object is a process, false = object is a thread
 * @param 3 pointer to process/thread state
 * @param 4 pointer to callback function (will be called before the thread/process wakes up) or NULL if there is no need for a callback
 * @return void
 **/
void add_trigger(trigger_t type, uint32_t ID, bool proc, void *object, void (*callback)(struct cpu_state **cpu))
{
    struct trigger_entry *new_entry = (struct trigger_entry*) malloc(sizeof(struct trigger_entry));
    new_entry->type   = type;
    new_entry->ID     = ID;
    new_entry->proc   = proc;
    new_entry->object = object;
    new_entry->callback = callback;
    list_push_front(trigger_list,new_entry);
}


/**
 * adds a interrupt trigger which activates a thread immediately
 * @param 0 IRQ number
 * @param 1 pointer to the thread (optional NULL)
 * @param 2 callback function (optional NULL)
 * @return true = sucess, false = failure
 **/
int add_int_trigger(int irq, struct thread_state *object,void (*callback)(int irq))
{
    if(install_irq(irq,handle_interupts))
    {
        add_trigger(WAIT_INT, irq, false, object,(void (*)(struct cpu_state **cpu))callback);
        return true;
    }
    return false;
}


/**
 * pulls a interrupt trigger
 * @param 0 cpu-state of the current process
 * @return void
 **/
void handle_interupts(struct cpu_state **cpu)
{
    iterator_t it = iterator_create(trigger_list);
    list_set_first(&it);
    while(!list_is_empty(trigger_list) && !list_is_last(&it))
    {
        struct trigger_entry *trg = list_get_current(&it);
        list_next(&it);
        if(trg->type == WAIT_INT && trg->ID == (*cpu)->intr-IRQ_OFFSET)
        {
            if(trg->callback)
            {
                ((void (*)(int irq))trg->callback)(trg->ID);
            }

            if(trg->object && ! (((struct thread_state *)trg->object)->flags & THREAD_ACTIV))
            {
                ((struct thread_state *)trg->object)->flags |= THREAD_ACTIV;
                list_insert_after(&it,trg->object);
                ((struct thread_state *)trg->object)->ticks = 10;
                current_thread->ticks = 0;
                *cpu = task_schedule(*cpu);
                return;
            }

        }

    }
}

/**
 * pulls waitpid triggers when a process has been killed
 * @param pointer to process state
 * @return void
 **/
void send_killed_process(struct process_state *proc)
{
    iterator_t it = iterator_create(trigger_list);
    list_set_first(&it);
    while(!list_is_empty(trigger_list) && !list_is_last(&it))
    {
        struct trigger_entry *current_entry = list_get_current(&it);

        if(current_entry->ID == proc->pid && current_entry->type == WAIT_PID) // pid > 0
        {
            if(current_entry->proc)
            {
                wakeup_process(current_entry->object);
                remove_event_trigger(current_entry->object, current_entry->ID);
            }
            else
            {
                wakeup_thread(current_entry->object);
                remove_event_trigger(current_entry->object, current_entry->ID);
            }

            list_remove(&it);
            free(current_entry);
            continue;
        }

        if(current_entry->ID == 0 && current_entry->type == WAIT_PID) // pid = 0
        {
            /*
                Process groups aren't implemented yet.
                Please rewrite this case.
            */

        }

        if((int)current_entry->ID == -1 && current_entry->type == WAIT_PID) // pid = -1
        {
            if(current_entry->object == proc->parent)
            {
                wakeup_process(proc->parent);
                remove_event_trigger(proc->parent, current_entry->ID);
            }
            else
            {
                iterator_t thread_it = iterator_create(proc->parent->threads);
                list_set_first(&thread_it);
                while(!list_is_empty(proc->parent->threads) && !list_is_last(&thread_it))
                {
                    struct thread_state *thread = list_get_current(&thread_it);
                    if(thread == current_entry->object)
                    {
                        wakeup_thread(thread);
                        remove_event_trigger(thread, current_entry->ID);
                    }
                    list_next(&thread_it);
                }
            }
        }

        if((int)current_entry->ID < -1 && current_entry->type == WAIT_PID) // pid < -1
        {
            /*
                Process groups aren't implemented yet.
                Please rewrite this case.
            */
        }
        list_next(&it);
    }

}



