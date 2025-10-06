#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

#define N_SAMPLES 10     // constant for the samples we should have when running the program
#define SLEEP_TIME 100   // constant for time taken between getting the ticks for each sample
#define A_TICKETS 30	 // constant for starting tickets for proc A
#define B_TICKETS 20     // constant for starting tickets for proc B
#define C_TICKETS 10	 // constant for starting tickets for proc C

void spin() {		 								// Function keeps process running by spinning
  int i, j;
  for(i = 0; i < 1000000; i++)
    j = i % 10; 	 								// prevent optimization
  (void)j;
}

int
main(int argc, char *argv[])
{
  int pidA; 
  int pidB;
  int pidC;
  struct pstat ps;									// Holds the stats from getpinfo
  int sampleNum = 0;									// Tracks the sample number we are on

  printf(1, "Starting scheduler graph test\n");
  printf(1, "Process A: %d tickets\n", A_TICKETS);
  printf(1, "Process B: %d tickets\n", B_TICKETS);
  printf(1, "Process C: %d tickets\n", C_TICKETS);
  printf(1, "Order for output: Sample number, ticks for A, ticks for B, ticks for C\n");  

  											// Parent should only have one ticket
  settickets(1);

  											// Create three children (A, B, C), and spin forever until killed
  pidA = fork();
  if(pidA == 0) 
  {
    settickets(A_TICKETS);
    for(;;) spin();
  }

  pidB = fork();
  if(pidB == 0) 
  {
    settickets(B_TICKETS);
    for(;;) spin();
  }

  pidC = fork();
  if(pidC == 0) 
  {
    settickets(C_TICKETS);
    for(;;) spin();
  }

  sleep(10);										// While children are spinning, parent sleeps so that children can start accumulating timeslices

  while(sampleNum < N_SAMPLES) 								// Loop through each sample, giving stats each loop
  {
    sleep(SLEEP_TIME);									// Sleep time between samples
        
    if(getpinfo(&ps) < 0) 								// call getpinfo and check that it succeeds
    {
      printf(1, "getpinfo failed\n");
      break;
    }
        										// Tick/timeslice counters are initialized for the current sample, if any of them stay at -1 by the end, it means the process was not found
    int timeslicesA = -1;
    int timeslicesB = -1; 
    int timeslicesC = -1;
    int i;

    for(i = 0; i < NPROC; i++) 								// Loop through processes to find A, B, and C
    {
      if(ps.pid[i] == pidA) 								// Once a process is found its timeslices are copied
      {
        timeslicesA = ps.ticks[i];
      } 
      else if(ps.pid[i] == pidB) 
      {
        timeslicesB = ps.ticks[i];
      } 
      else if(ps.pid[i] == pidC) 
      {
        timeslicesC = ps.ticks[i];
      }
    }
        
    if(timeslicesA >= 0 && timeslicesB >= 0 && timeslicesC >= 0) 			// The sample info only printed if all processes were found
    {
      printf(1, "%d,%d,%d,%d\n", sampleNum, timeslicesA, timeslicesB, timeslicesC);
      sampleNum++;									// Increment to next sample
    }
  }
    
    											// Kill each child process
  kill(pidA);
  kill(pidB);
  kill(pidC);
    
    											// Wait for children to exit
  wait();
  wait();
  wait();
    
  exit();
}
   
