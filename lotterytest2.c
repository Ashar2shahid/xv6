#include "types.h"
#include "user.h"

// test the lottery ticket scheduler
// This test makes sure that the process with the highest priority gets to run the most.

int
main(int argc, char *argv[])
{
  printf(1, "lottery test 2\n");

  // fork into 4 processes with different niceness values.
  int k = 0;
  int pid;

  pid = fork();
  if (pid == 0) {
    while(1) {
      // set priority to 20
      if (nice(0) <= 17)
        nice(1); // increment nice by 1
      if (nice(0) == 18) {
        k++;
        printf(1, "Process 1 -Priority=%d -tickets= %d -ticks=%d \n",nice(0),20-nice(0)+1,k);
      }
    }
  }
  pid = fork();
  if (pid == 0) {
    while(1) {
      // set priority to 16
      if (nice(0) <= 15)
        nice(1); // increment nice by 1
      if (nice(0) == 16) {
        k++;
        printf(1, "Process 2 -Priority=%d -tickets= %d -ticks=%d \n",nice(0),20-nice(0)+1,k);
      }
    }
  }
  pid = fork();
  if (pid == 0) {
    while(1) {
      // set priority to 11
      if (nice(0) <= 10)
        nice(1); // increment nice by 1
      if (nice(0) == 11) {
        k++;
        printf(1, "Process 3 -Priority=%d -tickets= %d -ticks=%d \n",nice(0),20-nice(0)+1,k);
      }
    }
  }
  else {
    while(1) {
      // set priority to 6
      if (nice(0) <= 5)
        nice(1); // increment nice by 1
      if (nice(0) == 6) {
        k++;
        printf(1, "Process 4 -Priority=%d -tickets= %d -ticks=%d \n",nice(0),20-nice(0)+1,k);
      }
    }
  }

  exit();
}