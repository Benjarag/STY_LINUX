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
    struct QueueItem *newNode = (struct QueueItem *)malloc(sizeof(struct QueueItem));
    if (!newNode) {
        printf("Memory allocation failed\n");
        return;
    }
    newNode->task = task; // append the task to the new node
    newNode->next = NULL;

    if (!listHead) {
        listHead = newNode;
        return;
    }

    struct QueueItem *tempItem = listHead;

    while (tempItem->next) {
        tempItem = tempItem->next;
    }
    tempItem->next = newNode;
    //...implement this
    // append to the end of the list
}

void *removeFirstItem()
{
    if (!listHead) {
        return NULL;
    }
    
    void *temp_task = listHead->task;

    struct QueueItem *nextItem = listHead->next;

    free(listHead);

    listHead = nextItem;
    
    return temp_task;

    //implement this
    // removes the first list item from the list and returns its value; returns NULL if list is empty
}

int containsItem(void *task)
{
    struct QueueItem *tempItem = listHead;
    while (tempItem) {
        if (tempItem->task == task)
            return 1;
        tempItem = tempItem->next;
    }
    return 0;
    //implement this
    // return true(1) if list contains value, false(0)
}

int isEmpty() {
    if (listHead == NULL) {
        return 0;
    }
    return 1;
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
