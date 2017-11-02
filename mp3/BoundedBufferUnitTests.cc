#include <iostream>
#include <cassert>
#include "BoundedBuffer.h"
#include <pthread.h>

using namespace std;

BoundedBuffer buffer(100);

void *produce_items(void *) {
    for (int i = 0; i < 100000; i++) { /* Deposit 100000 items */
        cout << "P: depositing " << i << endl;
        buffer.Deposit(i);
    }
    buffer.Deposit(-1); /* Indicate that this is the end */
    cout << "P: done" << endl;
    return 0;
}

void *consume_items(void *) {
    int val;
    do {
        val = buffer.Remove();
        if (val >= 0) {
            cout << "C: Removed " << val << endl;
        }
    } while (val >= 0);
    cout << "C: done" << endl;
    return 0;
}

int main() {
    pthread_t producer;
    pthread_t consumer;
    /* The asserts force the program to crash if the predicate is not satisfied,
       i.e., if errors are returned. */
    assert(pthread_create(&producer, NULL, produce_items, NULL) == 0);
    assert(pthread_create(&consumer, NULL, consume_items, NULL) == 0);
    assert(pthread_join(producer, NULL) == 0);
    assert(pthread_join(consumer, NULL) == 0);
    cout << "DONE!" << endl;
    return 0;
}