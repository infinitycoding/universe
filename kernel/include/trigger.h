#ifndef _trigger_h_
#define _trigger_h_


#include <stdint.h>
#include <process.h>

typedef enum
{
    WAIT_PID = 0,
    WAIT_TID = 1,
    WAIT_EVENT = 2,
    WAIT_TIME = 3
}trigger_t;

struct trigger_entry
{
    uint32_t type;
    uint32_t ID; // event ID or the parameter of waitpid
    bool proc;
    void *object; // pointer to the thread or Process struct. Depends on bool proc
};


void INIT_TRIGGER(void);
void wakeup_thread(struct thread_state *object);
void wakeup_process(struct process_state *object);
int remove_event_trigger_by_object(void *object, uint32_t ID);
int remove_event_trigger_by_id(uint32_t ID);
int send_event(uint32_t ID);
uint32_t add_event_trigger(bool proc, void *object);
void add_trigger(trigger_t type, uint32_t ID, bool proc, void *object);
void send_killed_process(struct process_state *proc);
#endif
