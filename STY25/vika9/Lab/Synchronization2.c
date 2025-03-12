#include <pthread.h>
#include <stdio.h>
pthread_mutex_t lock;

int value = 10;

void initialize () {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&lock, &attr);
}

void modify() {
    // modify the value if it is 100
    pthread_mutex_lock(&lock);
    if (value==100) {
        value = 52;
    }
    pthread_mutex_unlock(&lock);
}

void dosomething() {
    pthread_mutex_lock(&lock);
    // do the operations while holding the lock one time
    value = value + 10;
    modify(); // not another lock here
    value = value - 10;
    pthread_mutex_unlock(&lock);
}

int main() {
    printf("Value before: %d\n", value);
    initialize();
    dosomething();
    printf("Value after: %d\n", value);
}

// how to run:
// gcc -o sync2.out Synchronization2.c -lpthread
// ./sync2.out