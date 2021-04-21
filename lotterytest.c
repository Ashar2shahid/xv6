#include "types.h"
#include "user.h"

// test the lottery ticket scheduler
// This test makes sure that the process with the highest priority gets to run the most.
// This test also makes sure that processes with a priority of 0 (lowest possible) still gets to run.

int
main(int argc, char *argv[])
{
  printf(1, "lottery test\n");

  // fork into 4 processes with different niceness values.
  int k = 0;
  int pid;

  pid = fork();
  if (pid == 0) {
    while(1) {
      // keep priority between 12 and 15
      if (nice(0) <= 12)
        nice(5); // increment nice by 5
      k++;
      printf(1, "LOW - ticks - %d \n",k);
    }
  }

  pid = fork();
  if (pid == 0) {
    while(1) {
      // keep priority between 7 and 10
      if (nice(0) <= 7)
        nice(5); // increment nice by 5
      k++;
      printf(1, "MEDIUM - ticks - %d \n",k);
    }
  }

  pid = fork();
  if (pid == 0) {
    while(1) {
      // keep priority between 2 and 5
      if (nice(0) <= 2)
        nice(5); // increment nice by 5
      k++;
      printf(1, "HIGH - ticks - %d \n",k);
    }
  }

  while(1) {
      // keep priority between 17 and 20
      if (nice(0) <= 17)
        nice(5); // increment nice by 5
    k++;
    printf(1, "BAD - ticks - %d \n",k);
  }

  exit();
}