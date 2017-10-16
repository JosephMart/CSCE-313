#include "semaphore.H"

Semaphore::Semaphore(int _val) {
    value = _val;
}

Semaphore::~Semaphore() {
    mutex.Unlock();
    delay.Unlock();
}

void Semaphore::P() {
    mutex.Lock();
    value--;
    if (value < 0) {
        mutex.Unlock();
        delay.Lock();
    } else {
        mutex.Unlock();
    }
}

void Semaphore::V() {
    mutex.Lock();
    value++;
    if (value <= 0) {
        delay.Unlock();
    }
    mutex.Unlock();
}
