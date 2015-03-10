#ifndef _driver_h_
#define _driver_h_

#include <list.h>

#define running 1
#define online 2

struct driver
{
  struct driver *root;
  list_t *children;
  const char *file;
  int pid;
  int status;
};


typedef struct driver driver;


struct crossbar
{
  driver *d1;
  driver *d2;
  char *path1;
  char *path2;
  int status;
};

typedef struct crossbar crossbar;

struct poolClient
{
  driver *node;
  int status;
};

typedef struct poolClient poolClient;

struct pool
{
  list_t *clients;
  int status;
};

typedef struct pool pool;

driver *driverCreate(const char *file);
int driverLaunch(driver *drv, int rootPid);
int driverLink(driver *root, driver *child);

int crossbarAdd(list_t *barlist, driver *d1, driver *d2);
int crossbarLaunch(list_t *bars);

pool *poolCreate();
int poolAddClient(pool *p, driver *d);
int poolLaunch(pool *p);

#endif