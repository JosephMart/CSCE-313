#ifndef MP3_BOUNDEDBUFFER_H
#define MP3_BOUNDEDBUFFER_H

#include "semaphore.H"
#include "mutex.H"
//#include "mutex_guard.H"
#include <queue>

class BoundedBuffer {
private:
    /* -- INTERNAL DATA STRUCTURES
      You may need to change them to fit your implementation. */
    Semaphore *lock;
    Semaphore *full;
    Semaphore *empty;
    Mutex mutex;
    std::queue<int> q;
    int size;

public:
    /* -- CONSTRUCTOR/DESTRUCTOR */
    BoundedBuffer(int _size);

    ~BoundedBuffer();

    /* -- BUFFER OPERATIONS */
    void Deposit(int _value);

    int Remove();
};


#endif //MP3_BOUNDEDBUFFER_H
