/* 
    File: memtest.c

    Author: R. Bettati
            Department of Computer Science and Engineering
            Texas A&M University
    Date  : 16/06/27

    Test program for the memory allocator in MP1 of CSCE 313.
    This program reads and processes the command-line arguments and then
    exercises the memory allocator by allocating and de-allocating memory
    as part of the recursive invocations in the function 'ackerman', which 
    is famous for being very highly recursive.
*/
/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */
    
/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/time.h>
#include<assert.h>

#include "my_allocator.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */


/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

void ackerman_main();
/* Repeatedly asks user for two parameters, 'a' and 'b', and computes the result of the
   (highly recursive!) ackerman function. During every recursion step,
   it allocates and de-allocates a portion of memory with the use of the
   memory allocator defined in module "my_allocator.H".
*/ 

int ackerman(int a, int b);
/* This is the implementation of the Ackerman function. The function itself is very
   simple (just two recursive calls). We use it to exercise the memory allocator 
   (see "my_alloc" and "my_free"). 
   We use two additional calls to "gettimeofday" to measure the elapsed time.
*/

int num_allocations;
/* Global counter to keep track of the number of allocations exercised
   during one test run.
*/

void print_time_diff(struct timeval * tp1, struct timeval * tp2);
/* Prints to stdout the difference, in seconds and museconds, between
   two timevals. 
*/

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char ** argv) {

  // Add code to process input parameters (basic block size, memory length)

  // Initialize the allocator by calling: init_allocator(basic block size, memory length)

  ackerman_main();

  // Release the memory in the allocator by calling: release_allocator()

}

/*--------------------------------------------------------------------------*/
/* DRIVER FOR THE ACKERMAN FUNCTION */
/*--------------------------------------------------------------------------*/

void ackerman_main() {
  /* This function repeatedly first asks the user for the two parameters
     'a' and 'b' to pass to the Ackerman function, and then invokes the 
     Ackerman function.
     Before and after the invocation of the ackerman function, the 
     value of the wallclock is taken, and the elapsed time for the computation
     of the ackerman function is displayed.
  */

  int a, b; /* Parameters for the invocation of the Ackerman function. */ 

  struct timeval tp_start; /* Used to compute elapsed time. */
  struct timeval tp_end;

  for (;;) { /* Loop forewer, or until we break. */

    num_allocations = 0;

    printf("\n");
    printf("Please enter parameters 'a' and 'b' to ackerman function.\n");
    printf("Note that this function takes a long time to compute,\n");
    printf("even for small values. Keep 'a' at or below 3, and 'b' at or\n");
    printf("below 8. Otherwise, the function takes seemingly forever.\n");
    printf("Enter 0 for either 'a' or 'b' in order to exit.\n\n");

    printf("  a = "); scanf("%d", &a);
    if (!a) break;
     printf("  b = "); scanf("%d", &b);
    if (!b) break;

    printf("      a = %d, b = %d\n", a, b);

    assert(gettimeofday(&tp_start, 0) == 0);
    /* Assert aborts if there is a problem with gettimeofday.
       We rather die of a horrible death rather than returning
       invalid timing information! 
    */

    int result = ackerman(a, b); /* We really don't care about the result. It's like Zen:
                                    It's the way there that counts; not the result that we find there. */

    assert(gettimeofday(&tp_end, 0) == 0);
    /* (see above) */

    printf("Result of ackerman(%d, %d): %d\n", a, b, result); 

    printf("Time taken for computation : "); 
    print_time_diff(&tp_start, &tp_end);
    printf("\n");

    printf("Number of allocate/free cycles: %lu\n\n\n", num_allocations); 
  }
  
  printf("Reached end of Ackerman program. Thank you for using it.\n");

}

/*--------------------------------------------------------------------------*/
/* IMPLEMENTATION OF ACKERMAN FUNCTION */
/*--------------------------------------------------------------------------*/

int ackerman(int a, int b) {
/* This is the implementation of the Ackerman function. The function itself is very
   simple (just two recursive calls). We use it to exercise the memory allocator 
   (see "my_alloc" and "my_free"). 
   We use two additional calls to "gettimeofday" to measure the elapsed time.
*/

  /* Choose randomly the size of the memory to allocate: */
  int to_alloc =  ((2 << (rand() % 19)) * (rand() % 100)) / 100;
  if  (to_alloc < 4) to_alloc = 4;

  int result = 0;
  
  /* Here we allocate the memory using our own allocator. */
  char * mem = (char*)my_malloc(to_alloc * sizeof(char));

  num_allocations++;

  if (mem == NULL) {

    printf("ALLOCATOR FAILED: PROBABLY OUT OF MEMORY.\n");
    
  } else {

    /* Fill the allocated memory with a value. This same value will be used later for testing. */
    char c = rand() % 128;
    memset(mem, c, to_alloc * sizeof(char));

    /* Here we do the Ackerman recursion thing. */
    if (a == 0)
      result = b + 1;
    else if (b == 0)
      result = ackerman(a - 1, 1);
    else
      result = ackerman(a - 1, ackerman(a, b - 1) );

    /* We returned from recursion. Now let's check if the memory allocated above still has the same value. */
    for (int i = 0; i < to_alloc; i++) {
        if (mem[i] != c) {
	    /* The value does not match. It must have been overwritten somewhere. This is very bad. */
	    printf("Memory checking error!\n");
	    break;
        }
    }

    /* Now we free the memory allocated above; we use our own allocator. */
    my_free(mem);
  }

  return result; /* Generally the Ackerman function returns a value. We don't really care for it. */
}

/*--------------------------------------------------------------------------*/
/* UTILITY FUNCTIONS */
/*--------------------------------------------------------------------------*/

void print_time_diff(struct timeval * tp1, struct timeval * tp2) {
  /* Prints to stdout the difference, in seconds and museconds, between
     two timevals. 
  */

  long sec = tp2->tv_sec - tp1->tv_sec;
  long musec = tp2->tv_usec - tp1->tv_usec;
  if (musec < 0) {
    musec += 1000000;
    sec--;
  }
  printf(" [sec = %ld, musec = %ld] ", sec, musec);

}


