#include "types.h"
#include "user.h"

// test the lottery ticket scheduler
// This test increases the niceness value after a period of time to make sure that the process with the highest priority gets to run the most after changing the priority.

int
main(int argc, char *argv[])
{
  printf(1, "lottery test 3\n");

  // fork into 2 processes with different niceness values.
  int k = 0;
  int pid;

  pid = fork();
  if (pid == 0) {
    while(1) {
      if (nice(0) <= 2)
        nice(1); // set priority to 3
        if (k == 1000)
            nice(10); // Half the number of tickets
        k++;
        printf(1, "Process 1 -Priority=%d -tickets= %d -ticks=%d \n",nice(0),20-nice(0)+1,k);
    }
  }
  else {
    while(1) {
      if (nice(0) <= 2)
        nice(1); // set priority to 3
        k++;
        printf(1, "Process 2 -Priority=%d -tickets= %d -ticks=%d \n",nice(0),20-nice(0)+1,k);
    }
  }

  exit();
}