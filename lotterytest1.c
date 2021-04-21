#include "types.h"
#include "user.h"

// test the lottery ticket scheduler
// This test makes sure that the process with the highest priority gets to run the most.

int
main(int argc, char *argv[])
{
  printf(1, "lottery test 1\n");

  // fork into 2 processes with different niceness values.
  int k = 0;
  int pid;

  pid = fork();
  if (pid == 0) {
    while(1) {
      // set priority to 11
      if (nice(0) <= 10)
        nice(1); // increment nice by 1
      if (nice(0) == 11) {
        k++;
        printf(1, "Low Priority Process -Priority=%d -tickets= %d -ticks=%d \n",nice(0),20-nice(0)+1,k);
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
        printf(1, "High Priority Process -Priority=%d -tickets= %d -ticks=%d \n",nice(0),20-nice(0)+1,k);
      }
    }
  }

  exit();
}