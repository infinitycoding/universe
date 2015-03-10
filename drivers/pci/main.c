#include <pci.h>

#include <udrcp.h>
#include <udrcp/ioport.h>


port_t *address_port;
port_t *data_port;

int main(void)
{
    
    int logfile = open("/var/log/drivers/pci.log", O_WRONLY, 0);
    
    pckmgr *conn = new_pckmgr();
    if(!subsystem_connect(conn,HYPERVISOR,"udrcp",logfile,UHOST_DEFAULT_SYNCHRON))
    {
        udrcp_error(conn,"could not connect to host\n");
        return -1;
    }

    
    data_port = port_alloc(conn,3320,4);
    address_port = port_alloc(conn,3320,4);
    INIT_PCI();
    exit(0);

    if(!address_port || !data_port)
    {
        udrcp_error(conn,"could not allocate port");
        return -2;
    }

    while(1)
    {
        pck_t *signal = poll_next(conn);
        switch(signal->type)
        {

            case RESET_CON:
                reset_conn(conn);
            break;

            case SHUTDOWN:
                printf("shutdown!\n");
                while(1);
            break;

            case RESTART:
            case CHKDEV:
                udrcp_error(conn,"unusual signal\n");
            break;

            default:
                udrcp_error(conn,"unknown signal\n");
            break;
        }
        free_pck(signal);
    }

    return 0;
}
