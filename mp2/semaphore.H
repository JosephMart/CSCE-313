/* 
    File: semaphore.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/11

*/

#ifndef _semaphore_H_                   // include file only once
#define _semaphore_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <pthread.h>
#include "mutex.H"
#include "mutex_guard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   S e m a p h o r e  */
/*--------------------------------------------------------------------------*/

class Semaphore {
private:
    /* -- INTERNAL DATA STRUCTURES
       You may need to change them to fit your implementation. */

    int value;
    Mutex mutex = Mutex(false);
    Mutex delay = Mutex(true);
    MutexGuard mutex_g = MutexGuard(mutex);
    MutexGuard delay_g = MutexGuard(delay);
public:

    /* -- CONSTRUCTOR/DESTRUCTOR */

    Semaphore(int _val);

    ~Semaphore();

    /* -- SEMAPHORE OPERATIONS */

    void P();

    void V();
};


#endif


