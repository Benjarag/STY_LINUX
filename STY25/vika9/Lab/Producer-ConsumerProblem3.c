#include <semaphore.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#define BUFFER_SIZE 10
int buffer [BUFFER_SIZE];
int buffer_index = 0; // Current element in buffer

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

void init() {
    sem_init(&empty, 0, BUFFER_SIZE); // All slots initially free
    sem_init(&full, 0, 0);            // No items yet
    pthread_mutex_init(&mutex, NULL);
}

void produce(int item) {
    sem_wait(&empty);              // Wait for a free slot
    pthread_mutex_lock(&mutex);    // Enter critical section

    // Critical section: add item to buffer
    buffer[buffer_index] = item;
    buffer_index++;                // Increment index (could be made circular)

    pthread_mutex_unlock(&mutex);  // Exit critical section
    sem_post(&full);               // Signal that an item is available
}

int consume() {
    int item;
    sem_wait(&full);               // Wait for an available item
    pthread_mutex_lock(&mutex);    // Enter critical section

    // Critical section: remove item from buffer
    buffer_index--;                // Decrement index (again, could be circular)
    item = buffer[buffer_index];

    pthread_mutex_unlock(&mutex);  // Exit critical section
    sem_post(&empty);              // Signal that a slot is free

    return item;
}

int main() {
    init();
    produce(10);
    printf("Consumed: %d\n", consume());
    return 0;
}
// What is the output?
// Answer: Consumed: 10

/*
Explanation:
The producer waits until there’s an empty slot before entering its critical section.
It then locks the mutex, writes to the buffer, increments the index, unlocks the mutex, 
and signals that a new item is available. 
The consumer does the opposite—waiting until there is an item (full), 
then entering the critical section to remove it, updating the index, 
and finally signaling that there is an empty slot available.
*/
