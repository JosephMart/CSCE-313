#include "mutex.H"

Mutex::Mutex()
{
    pthread_mutex_init(&(this->m), NULL);
}

Mutex::Mutex(bool l)
{
    pthread_mutex_init(&(this->m), NULL);
    this->locked = l;
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&(this->m));
}

void Mutex::Lock()
{
    pthread_mutex_lock(&(this->m));
    this->locked = true;
}

void Mutex::Unlock()
{
    pthread_mutex_unlock(&(this->m));
    this->locked = false;
}
