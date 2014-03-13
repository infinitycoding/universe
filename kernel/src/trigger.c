#include <trigger.h>
#include <list.h>
#include <heap.h>

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

    list_set_first(running_threads);
    while(list_get_current(running_threads) != object && !list_is_last(running_threads))
        list_next(running_threads);

    if(list_get_current(running_threads) == object)
    {
        list_remove(running_threads);
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

    list_set_first(object->threads);
    while(!list_is_last(object->threads))
    {
        struct thread_state *thread = (struct thread_state *)list_get_current(object->threads);
        if(thread->flags & THREAD_ACTIV)
            suspend_thread(thread);

        list_next(object->threads);
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
    list_set_first(process->threads);
    while(!list_is_empty(process->threads) && !list_is_last(process->threads))
    {
        struct thread_state *thread = (struct thread_state *)list_get_current(process->threads);
        if(!(thread->flags & THREAD_ACTIV))
            wakeup_thread(thread);
        list_next(process->threads);
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
 * Removes every trigger with the given ID
 * @param 0 event ID
 * @return number of removed triggers, 0 if there was no trigger with the given ID
 **/
int remove_event(uint32_t ID)
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
    int ret = false;
    list_set_first(trigger_list);
    while(!list_is_last(trigger_list) && !list_is_empty(trigger_list))
    {
        struct trigger_entry *current_entry = list_get_current(trigger_list);
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
		
                if(current_entry->callback != NULL) {
			struct cpu_state **cpu = &thread->state;
			struct thread_state *tmp = current_thread;
			current_thread = thread;
			vmm_switch_context(&current_thread->context);
                	current_entry->callback(cpu);
			current_thread = tmp;
			vmm_switch_context(&current_thread->context);
                }
            }

            remove_event(ID);
            ret = true;
        }
        list_next(trigger_list);
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
    if(list_is_empty(event_id_list))
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
                remove_event_trigger(current_entry->object, current_entry->ID);
            }
            else
            {
                wakeup_thread(current_entry->object);
                remove_event_trigger(current_entry->object, current_entry->ID);
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
                remove_event_trigger(proc->parent, current_entry->ID);
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
                        remove_event_trigger(thread, current_entry->ID);
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



