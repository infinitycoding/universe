#ifndef _task_h_
#define _task_h_
#include <thread.h>
#include <paging.h>

#define TASK_ACTIV 1
#define TASK_FREEZED 2
#define TASK_KERNELMODE 4
#define TASK_NORMAL_PRIORITY 8
#define TASK_REALTIME_PRIORITY 16
#define TASK_ZOMBIE 32

#define ERROR_TASK_NOT_FOUND -1
typedef uint32_t pid_t;


typedef enum{
    background = 0,
    normal     = TASK_NORMAL_PRIORITY,
    realtime   = TASK_REALTIME_PRIORITY,

}priority_t;

typedef enum{
    user_mode =0,
	kernel_mode = 1,
}prev_t;

struct port_access
{
    struct port_access *prev;
    struct port_access *next;
    uint16_t portnum;
};


struct child
{
    struct child *prev;
    struct child *next;
    int return_value;
    pid_t pid;
    struct task_state *proc;
};


struct zombiepid
{
    struct zombiepid *prev;
    struct zombiepid *next;
    pid_t pid;
};



#define TASK_STATE_STRUCT_SIZE 564
struct task_state
{
	struct task_state *prev;
	struct task_state *next;
	pid_t pid;
	pd_t *pagedir;
	char name[256];
	char desc[256];
	struct port_access *port;
	struct task_state* proc_parent;
	struct thread *threads;
	struct thread *currentthread;
	tid_t tid_counter;
	struct zombietid *freetid;
	struct child *proc_children;
	uint32_t flags;
};


pid_t get_freepid(void);
int proc_activate(pid_t pid);
void zombiepid_add(pid_t pid);
pid_t zombiepid_get();
void proc_structure_free(struct task_state *proc);
int proc_kill(struct task_state *proc);
void exit(int errorcode);
struct task_state* proc_get(pid_t pid);
pid_t proc_create(char* name, char* desc, pd_t *pd, prev_t prev, priority_t priority);

#endif
