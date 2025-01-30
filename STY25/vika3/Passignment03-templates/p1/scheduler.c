#include "scheduler.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

static Queue priorityQueues[MAX_PRIORITY + 1];
static int currentPriority = 0;
static int samePrioCounter[MAX_PRIORITY + 1] = {0};

void appendItem(Queue *queue, void *data) {
    QueueItem *newNode = malloc(sizeof(QueueItem));
    if (!newNode) return;

    newNode->data = data;
    newNode->next = NULL;

    if (queue->tail) {
        queue->tail->next = newNode;
    } else {
        queue->head = newNode;
    }
    queue->tail = newNode;
}

void *removeFirstItem(Queue *queue) {
    if (!queue->head) return NULL;

    void *temp_data = queue->head->data;
    QueueItem *nextItem = queue->head->next;
    free(queue->head);
    queue->head = nextItem;

    if (!queue->head) {
        queue->tail = NULL;
    }
    return temp_data;
}

void initScheduler() {
    for (int i = 0; i <= MAX_PRIORITY; i++) {
        priorityQueues[i].head = NULL;
        priorityQueues[i].tail = NULL;
        samePrioCounter[i] = 0;
    }
    currentPriority = 0;
}

void onTaskReady(Task *task) {
    if (task->state == STATE_READY) return;

    if (task->priority < 0) task->priority = 0;
    else if (task->priority > MAX_PRIORITY) task->priority = MAX_PRIORITY;

    task->state = STATE_READY;
    appendItem(&priorityQueues[task->priority], task);
}

void onTaskPreempted(Task *task) {
    if (task->state != STATE_RUNNING) return;
    task->state = STATE_READY;
    appendItem(&priorityQueues[task->priority], task);
}

void onTaskWaiting(Task *task) {
    if (task->state != STATE_RUNNING) return;
    task->state = STATE_WAITING;
}

Task *scheduleNextTask() {
    if (samePrioCounter[currentPriority] >= MAX_SAMEPRIO_LENGTH) {
        for (int p = currentPriority + 1; p <= MAX_PRIORITY; p++) {
            if (priorityQueues[p].head) {
                Task *task = removeFirstItem(&priorityQueues[p]);
                if (task) {
                    samePrioCounter[currentPriority] = 0;
                    currentPriority = p;
                    samePrioCounter[p] = 1;
                    task->state = STATE_RUNNING;
                    return task;
                }
            }
        }
        samePrioCounter[currentPriority] = 0;
    }

    for (int p = 0; p <= MAX_PRIORITY; p++) {
        if (priorityQueues[p].head) {
            Task *task = removeFirstItem(&priorityQueues[p]);
            if (!task) continue;

            if (p == currentPriority) {
                samePrioCounter[p]++;
            } else {
                samePrioCounter[currentPriority] = 0;
                currentPriority = p;
                samePrioCounter[p] = 1;
            }

            task->state = STATE_RUNNING;
            return task;
        }
    }

    return NULL;
}