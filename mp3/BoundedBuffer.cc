#include <pthread.h>
#include <queue>
#include <string>
#include "semaphore.H"
#include "mutex.H"
#include "BoundedBuffer.h"

using namespace std;

BoundedBuffer::BoundedBuffer(int _size) {
    size = _size;
    lock = new Semaphore(1);
    full = new Semaphore(0);
    empty = new Semaphore(size);
}

BoundedBuffer::~BoundedBuffer() {
    delete lock;
    delete full;
    delete empty;
}

void BoundedBuffer::Deposit(int _value) {
    empty->P();  //P() decreases
    lock->P();   // Crit Section
    q.push(_value);
    lock->V();   // V() increases
    full->V();
}


int BoundedBuffer::Remove() {
    full->P();
    lock->P();                   //Crit Section
    int r = q.front();
    q.pop();
    lock->V();
    empty->V();
    return r;
}
