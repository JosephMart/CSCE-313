#ifndef MP3_BOUNDEDBUFFER_H
#define MP3_BOUNDEDBUFFER_H

#include "semaphore.H"
#include "mutex.H"
#include <queue>
#include <pthread.h>

using namespace std;

template <class T>
class BoundedBuffer {
private:
    /* -- INTERNAL DATA STRUCTURES
      You may need to change them to fit your implementation. */
    Semaphore *lock;
    Semaphore *full;
    Semaphore *empty;
    Mutex mutex;
    std::queue<T> q;
    int size;

public:
    /* -- CONSTRUCTOR/DESTRUCTOR */
    BoundedBuffer();
    BoundedBuffer(int _size);

    ~BoundedBuffer();

    /* -- BUFFER OPERATIONS */
    void Deposit(T _value);

    T Remove();
};

template <class T>
BoundedBuffer<T>::BoundedBuffer() {
    size = 0;
}

template <class T>
BoundedBuffer<T>::BoundedBuffer(int _size) {
    size = _size;
    lock = new Semaphore(1);
    full = new Semaphore(0);
    empty = new Semaphore(size);
}

template <class T>
BoundedBuffer<T>::~BoundedBuffer() {
    delete lock;
    delete full;
    delete empty;
}

template <class T>
void BoundedBuffer<T>::Deposit(T _value) {
    empty->P();
    lock->P();
    q.push(_value);
    lock->V();
    full->V();
}

template <class T>
T BoundedBuffer<T>::Remove() {
    full->P();
    lock->P();
    T r = q.front();
    q.pop();
    lock->V();
    empty->V();
    return r;
}


#endif //MP3_BOUNDEDBUFFER_H
