#ifndef _trigger_h_
#define _trigger_h_


#include <stdint.h>
#include <process.h>
#include <thread.h>

typedef enum
{
    WAIT_PID = 0,
    WAIT_TID = 1,
    WAIT_EVENT = 2,
    WAIT_TIME = 3,
    WAIT_PIPE = 4,
    WAIT_INT = 5,
}trigger_t;

struct trigger_entry
{
    uint32_t type;
    uint32_t ID; // event ID or the parameter of waitpid
    bool proc;
    void *object; // pointer to the thread or Process struct. Depends on bool proc
    void (*callback)(struct cpu_state **cpu);
};


void INIT_TRIGGER(void);
void suspend_thread(struct thread_state *object);
void suspend_process(struct process_state *object);
void wakeup_thread(struct thread_state *object);
void wakeup_process(struct process_state *object);
int remove_event_trigger(void *object, uint32_t ID);
int remove_event(uint32_t ID);
int send_event(uint32_t ID);
uint32_t get_new_event_ID(void);
uint32_t add_event_trigger(bool proc, void *object, void (*callback)(struct cpu_state **cpu));
void add_trigger(trigger_t type, uint32_t ID, bool proc, void *object, void (*callback)(struct cpu_state **cpu));
void send_killed_process(struct process_state *proc);
void handle_interupts(struct cpu_state **cpu);
int add_int_trigger(int irq, struct thread_state *object,void (*callback)(int irq));

#endif
