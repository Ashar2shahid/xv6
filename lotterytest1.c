#include "types.h"
#include "user.h"

// test the lottery ticket scheduler
// This test makes sure that the process with the highest priority gets to run the most.
// This test also makes sure that processes with a priority of 0 (lowest possible) still gets to run.

int
main(int argc, char *argv[])
{
  printf(1, "lottery test\n");

  // fork into 2 processes with different niceness values.
  int k = 0;
  int pid;

  pid = fork();
  if (pid == 0) {
    while(1) {
      // set priority to 9
      if (nice(0) <= 8)
        nice(1); // increment nice by 1
      if (nice(0) == 9) {
        k++;
        printf(1, "Low Priority Process -Priority=%d -tickets= %d -ticks=%d \n",nice(0),20-nice(0)+1,k);
      }
    }
  }
  else {
    while(1) {
      // set priority to 4
      if (nice(0) <= 3)
        nice(1); // increment nice by 1
      if (nice(0) == 4) {
        k++;
        printf(1, "High Priority Process -Priority=%d -tickets= %d -ticks=%d \n",nice(0),20-nice(0)+1,k);
      }
    }
  }

  exit();
}