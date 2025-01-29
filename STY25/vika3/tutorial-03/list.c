#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

struct Task { int pid; };

struct QueueItem {
    struct QueueItem *next;
    void *task;
};

struct QueueItem *listHead = NULL;

void appendItem(void *task)
{
    //...implement this
    // append to the end of the list
}

void *removeFirstItem()
{
    //implement this
    // removes the first list item from the list and returns its value; returns NULL if list is empty
}

int containsItem(void *task)
{
    //implement this
    // return true(1) if list contains value, false(0)
}

int isEmpty() {
    // implement this
    // return true (1) if list is empty, false (0) otherwise
}

struct Task t1 = {42}, t2 = {4711}, t3 = {123};
int main() {
    appendItem(&t1);
    appendItem(&t2);
    struct Task *next = removeFirstItem();
    printf("Next tid: %d\n", next==NULL ? -1 : next->pid);
    appendItem(&t1);
    appendItem(&t3);
    for(int i=0; i<5; i++) {
        struct Task *next = removeFirstItem();
        printf("Next tid: %d\n", next==NULL ? -1 : next->pid);
    }
}
