#include <driver.h>
#include <stdint.h>
#include <string.h>
driver *driverCreate(const char *file)
{
  driver *newDriver = (driver *)malloc(sizeof(driver));
  newDriver->file = file;
  newDriver->root = 0;
  newDriver->children = list_create();
  newDriver->pid = 0;
  newDriver->status = 0;
}


int driverLink(driver *root, driver *child)
{
  if(root == 0 || child == 0)
    return -1;
  list_push_front(root->children,child);
  child->root = root;
  return 0;
}

int driverLaunch(driver *drv, int rootPid)
{
  int pid;

  if((pid = fork()) == 0) 
  {
    const char *argv[4] = {0, 0, 0, 0};
    char root[10] = {0,0,0,0,0,0,0,0,0,0};
    argv[0] = drv->file;
    argv[1] = "-r";
    sprintf(root,"%d",rootPid);
    argv[2] = root;
    execve(drv->file,argv, 0);
  }
  else
  {
    iterator_t it = iterator_create(drv->children);
    while(!list_is_empty(drv->children) && !list_is_last(&it))
    {
     driver *currentChild = (driver *)list_get_current(&it);
     driverLaunch(currentChild,pid);
     list_next(&it); 
    }
  }
}

int crossbarAdd(list_t *barlist, driver *d1, driver *d2)
{
   if(d1 == 0 || d2 == 0)
    return -1;
   crossbar *newBar = (crossbar *)malloc(sizeof(crossbar));
   newBar->d1 = d1;
   newBar->d2 = d2;
   newBar->path1 = 0;
   newBar->path2 = 0;
   newBar->status = 0;
   return 0;
};

int crossbarLaunch(list_t *bars)
{
  if(bars == 0)
    return -1;
  iterator_t it = iterator_create(bars);
  int launchedBars = 0;
  while(!list_is_empty(bars) && !list_is_last(&it))
  {
    crossbar *bar =(crossbar *)list_get_current(&it);
    if(bar->d1->status == online && bar->d2->status == online)
    {
      //make fifos
      launchedBars++;
    }
    list_next(&it);
  }
  
  return launchedBars;
}

pool *poolCreate()
{
  pool *newPool = (pool *)malloc(sizeof(pool));
  if(!newPool)
    return 0;
  
  newPool->clients = list_create();
  newPool->status = 0;
  return newPool;
}


int poolAddClient(pool *p, driver *d)
{
  if(p == 0 || d == 0)
    return -1;
  poolClient *newClient = (poolClient *)malloc(sizeof(poolClient));
  newClient->status = 0;
  newClient->node = d;
  list_push_front(p->clients,newClient);
  return 0;
}

int poolLaunch(pool *p)
{
  if(p == 0)
    return -1;
  
  iterator_t it = iterator_create(p->clients);
  int connectedClients = 0;
  while(!list_is_empty(p->clients) && !list_is_last(&it))
  {
    poolClient *client1 = (poolClient *)list_get_current(&it);
    
    iterator_t it2 = iterator_create(p->clients);
    while(!list_is_last(&it2))
    {
      poolClient *client2 = (poolClient *)list_get_current(&it2);
      //todo: link 
      list_next(&it2);
    }
    connectedClients++;
    list_next(&it);
  }
  return connectedClients;
}

