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
    sem_wait(&empty);              // Wait for a empty slot

    // Directly write to buffer without mutex since no concurrent producer
    buffer[buffer_index] = item;
    buffer_index++;                // Increment index

    sem_post(&full);               // Signal that an item is available
}

int consume() {
    int item;
    sem_wait(&full);               // Wait for an available item

    // Directly read from buffer without mutex since no concurrent consumer
    buffer_index--;                // Decrement index (again, could be circular)
    item = buffer[buffer_index];

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
Because only one thread produces and one thread consumes, 
there is no risk of two producers or two consumers modifying
the shared index or buffer concurrently.
The semaphores alone ensure that the order of operations is maintained.
*/
