#include <trigger.h>
#include <list.h>
#include <heap.h>

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
 * Wakes up a thread
 * @param 0 pointer to the thread state
 * @return void
 **/
void wakeup_thread(struct thread_state *object)
{
    list_push_front(running_threads, object);
}


/**
 * Wakes up a process
 * @param 0 pointer to the process state
 * @return void
 **/
void wakeup_process(struct process_state *object)
{
    struct process_state *process = object;
    list_set_first(process->threads);
    while(!list_is_empty(process->threads) && !list_is_last(process->threads))
    {
        list_push_front(running_threads, list_get_current(process->threads));
        list_next(process->threads);
    }
}


/**
 * Removes a event trigger by object name and ID if given,
 * @param 0 pointer to thread/process state
 * @param 1 optional event ID (0 = don't care)
 * @return number or removed triggers
 **/
int remove_event_trigger_by_object(void *object, uint32_t ID)
{
    int removed_elements = 0;
    if(ID == 0)
    {
        list_set_first(trigger_list);
        while(!list_is_last(trigger_list) && !list_is_empty(trigger_list))
        {
            struct trigger_entry *current_entry = list_get_current(trigger_list);
            if(current_entry->object == object)
            {
                list_remove(trigger_list);
                if(current_entry->type == WAIT_EVENT)
                    list_push_front(event_id_list,(void*)current_entry->ID);
                free(current_entry);
                removed_elements++;
            }
            list_next(trigger_list);
        }
    }
    else
    {
        list_set_first(trigger_list);
        while(!list_is_last(trigger_list) && !list_is_empty(trigger_list))
        {
            struct trigger_entry *current_entry = list_get_current(trigger_list);
            if(current_entry->object == object && current_entry->ID == ID)
            {
                list_remove(trigger_list);
                if(current_entry->type == WAIT_EVENT)
                    list_push_front(event_id_list,(void*)current_entry->ID);
                free(current_entry);
                removed_elements++;
                break;
            }
            list_next(trigger_list);
        }
    }
    return removed_elements;
}


/**
 * Removes a event trigger by event ID
 * @param 0 event ID
 * @return true if the trigger was removed, false if there was no trigger with the given ID
 **/
int remove_event_trigger_by_id(uint32_t ID)
{
    int removed_elements = 0;
    list_set_first(trigger_list);
    while(!list_is_last(trigger_list) && !list_is_empty(trigger_list))
    {
        struct trigger_entry *current_entry = list_get_current(trigger_list);
        if(current_entry->ID == ID)
        {
            list_remove(trigger_list);
            if(current_entry->type == WAIT_EVENT)
                list_push_front(event_id_list,(void*)current_entry->ID);
            free(current_entry);
            removed_elements++;
            break;
        }

        list_next(trigger_list);
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
    list_set_first(trigger_list);
    while(!list_is_last(trigger_list) && !list_is_empty(trigger_list))
    {
        struct trigger_entry *current_entry = list_get_current(trigger_list);
        if(current_entry->ID == ID)
        {
            if(current_entry->proc)
            {
                wakeup_process(current_entry->object);
                remove_event_trigger_by_object(current_entry->object, current_entry->ID);
            }
            else
            {
                wakeup_thread(current_entry->object);
                remove_event_trigger_by_object(current_entry->object, current_entry->ID);
            }

            remove_event_trigger_by_id(ID);
            return true;
        }
        list_next(trigger_list);
    }
    return false;
}


/**
 * Adds a event trigger
 * @param 0 true = object is a process, false = object is a thread
 * @param 1 pointer to process/thread state
 * @return event ID
 **/
uint32_t add_event_trigger(bool proc, void *object)
{
    struct trigger_entry *new_entry = (struct trigger_entry*) malloc(sizeof(struct trigger_entry));
    new_entry->type = WAIT_EVENT;

    if(list_is_empty(event_id_list))
    {
        new_entry->ID = event_id_counter++;
    }
    else
    {
        new_entry->ID = (uint32_t) list_pop_back(event_id_list);
    }

    new_entry->proc = proc;
    new_entry->object = object;
    list_push_front(trigger_list,new_entry);
    return new_entry->ID;
}


/**
 * Adds a trigger
 * @param 0 trigger type
 * @param 1 trigger ID
 * @param 2 true = object is a process, false = object is a thread
 * @param 3 pointer to process/thread state
 * @return void
 **/
void add_trigger(trigger_t type, uint32_t ID, bool proc, void *object)
{
    struct trigger_entry *new_entry = (struct trigger_entry*) malloc(sizeof(struct trigger_entry));
    new_entry->type   = type;
    new_entry->ID     = ID;
    new_entry->proc   = proc;
    new_entry->object = object;
    list_push_front(trigger_list,new_entry);
}


/**
 * pulls waitpid triggers when a process has been killed
 * @param pointer to process state
 * @return void
 **/
void send_killed_process(struct process_state *proc)
{
    list_set_first(trigger_list);
    while(!list_is_empty(trigger_list) && !list_is_last(trigger_list))
    {
        struct trigger_entry *current_entry = list_get_current(trigger_list);

        if(current_entry->ID == proc->pid && current_entry->type == WAIT_PID) // pid > 0
        {
            if(current_entry->proc)
            {
                wakeup_process(current_entry->object);
                remove_event_trigger_by_object(current_entry->object, current_entry->ID);
            }
            else
            {
                wakeup_thread(current_entry->object);
                remove_event_trigger_by_object(current_entry->object, current_entry->ID);
            }

            list_remove(trigger_list);
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
                remove_event_trigger_by_object(proc->parent, current_entry->ID);
            }
            else
            {
                list_set_first(proc->parent->threads);
                while(!list_is_empty(proc->parent->threads) && !list_is_last(proc->parent->threads))
                {
                    struct thread_state *thread = list_get_current(proc->parent->threads);
                    if(thread == current_entry->object)
                    {
                        wakeup_thread(thread);
                        remove_event_trigger_by_object(thread, current_entry->ID);
                    }
                    list_next(proc->parent->threads);
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
        list_next(trigger_list);
    }

}



