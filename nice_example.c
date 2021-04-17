#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
    int  k, id;
    double i,x;
    x=0;
    id = 0;
    for (k = 0; k < 20;k++){
        id = fork();
        if(id < 0){
            printf(1,"%d failed to fork \n", getpid() );
        }
        else if (id > 0) {
            //In Parent Process 
            printf(1,"Parent %d creating child %d \n", getpid(), id);
            wait();
        }
        else {
            //In Child Process
            printf(1, "Child %d created \n", getpid());
            for (i=0;i<99999999;i++){
                x = x + 10.11 * 11.11; // Calculation to consume CPU time.
            }
            break;
        }
    }
    exit();
}