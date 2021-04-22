#include "types.h"
#include "user.h"

// test the lottery ticket scheduler
// This test makes sure that the process with the highest priority gets to run the most.

int
main(int argc, char *argv[])
{
  printf(1, "lottery test 1\n");

  // fork into 8 processes with 4 proccess having 10 tickets and 4 proccess having 15 tickets
  int k = 0;
  double z = 0;
  int pid;
  int i;

  for (i=0;i<4;i++){
    pid = fork();
    if (pid == 0) {
      while(1) {
        // set priority to 11
        if (nice(0) <= 10)
          nice(1); // increment nice by 1
        if (nice(0) == 11) {
          k++;
          z += 3.14 * 89.9; // occupy cpu time
          if(k%1000000 == 0)
          printf(1, "Low Priority Process %d -Priority=%d -tickets= %d -ticks=%d \n",i,nice(0),20-nice(0)+1,k);
        }
      }
    }
  }

  for (i=4;i<8;i++){
    pid = fork();
    if (pid == 0) {
      while(1) {
      // set priority to 6
      if (nice(0) <= 5)
        nice(1); // increment nice by 1
      if (nice(0) == 6) {
        k++;
        z += 3.14 * 89.9; // occupy cpu time
        if(k%1000000 == 0)
        printf(1, "High Priority Process %d -Priority=%d -tickets= %d -ticks=%d \n",i,nice(0),20-nice(0)+1,k);
        }
      }
    }
  }


  exit();
}