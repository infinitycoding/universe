#ifndef _trigger_h_
#define _trigger_h_


#include <stdint.h>

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




#endif
