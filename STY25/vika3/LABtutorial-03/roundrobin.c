#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "scheduler.h"

/*
 * Put your list implementation here (without the definition on struct Task,
 * this structure is now defined in scheduler.h
 */

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
        return 1;
    }
    return 0;
    // implement this
    // return true (1) if list is empty, false (0) otherwise
}

/* .... */

/*
 * Skeleton of scheduler implementation, you have to complete these two functions
 * The functions should
 * (a) add or remove tasks from your queue, as appropriate
 * (b) change the task state ("state" element in struct Task), as appropriate
 */

Task *scheduleNextTask()
{
    // TODO: implement
    // return and remove the first task if it exists
    if (isEmpty()) {
        return NULL;
    }
    return removeFirstItem();
}

void onTaskReady(Task *task) {
    // TODO: implement
    // append the given task to the ready queue,
    // only if it is not already in the queue,
    task->state = STATE_READY;
    if (!containsItem(task)) {
        appendItem(task);
    }
    return;
    // and update the state to STATE_READY
}

/*
 * New main program for round robin scheduler
 */

// state, taskid, priority, name
static Task tasks[] = {
 { STATE_NEW, 0, 0, "A" },
 { STATE_NEW, 1, 0, "B" },
 { STATE_NEW, 2, 0, "C" },
 { STATE_NEW, 3, 0, "D" },
};

int main() {
    // Let's indicate that all four tasks are ready
    onTaskReady(&tasks[0]);
    onTaskReady(&tasks[1]);
    onTaskReady(&tasks[2]);
    onTaskReady(&tasks[3]);

    // Now let's look at 16 iterations.... 
    // let the schedular schedule a task
    // ... the tasks run
    // ... tell the scheduler that the task has been preempted (by calling onTaskPreempted)
    for(int i=0; i<16; i++) {
        struct Task *task = scheduleNextTask();
        printf("Next task id: %d\n", task==NULL ? -1 : task->pid);
        onTaskReady(task);
    }
}
