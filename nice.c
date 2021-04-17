#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int 
main(int argc, char *argv[])
{
    int priority, pid;

    if(argc < 3){
        printf(2, "Need 2 arguments : pid priority \n");
        exit();
    }
    pid = atoi(argv[1]);
    priority = atoi(argv[2]);
    if (priority < 0) {
        printf(2, "Invalid priority\n");
        exit();
    }

    chprio(pid,priority);

    exit();
}