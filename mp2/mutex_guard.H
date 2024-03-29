/* 
    File: mutex_guard.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 07/14/16

*/

#ifndef _mutex_guard_H_                   // include file only once
#define _mutex_guard_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "mutex.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   M u t e x G u a r d */
/*--------------------------------------------------------------------------*/

class MutexGuard {
private:
    /* -- INTERNAL DATA STRUCTURES
       You may need to change them to fit your implementation. */

    Mutex *m;

public:

    /* -- CONSTRUCTOR/DESTRUCTOR */

    MutexGuard(Mutex &m) {
        this->m = &m;
        m.Lock();
    };

    ~MutexGuard() {
        m->Unlock();
    };

};


#endif


