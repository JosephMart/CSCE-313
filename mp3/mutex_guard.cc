#include "mutex.H"
#include "mutex_guard.H"

MutexGuard::MutexGuard(Mutex &m) {
    this->m = &m;
    m.Lock();
}

MutexGuard::~MutexGuard() {
    m->Unlock();
}
