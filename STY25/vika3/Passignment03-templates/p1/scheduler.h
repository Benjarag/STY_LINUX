#ifndef SCHEDULER_H
#define SCHEDULER_H


/* 
 * Helper functions for managing queues
 */

typedef struct QueueItem {
    /* A pointer to the data item stored in the queue */
    void *data;

    /* The next item in the queue, NULL if there is no next item.  */
    struct QueueItem *next;
} QueueItem;


typedef struct Queue {
    /* The first item of the queue; NULL if the queue is empty.  */
    QueueItem *head;
    /* The last item of the queue; NULL if the queue is empty.  */
    QueueItem *tail;
} Queue;

void appendItem(Queue *queue, void *task);
void *removeFirstItem(Queue *queue);


/*
 * The maximum priority.
 * Note: 0 is highest priorty, MAX_PRIORITY is the lowest priority
 */
#define MAX_PRIORITY 10 

/* For starvation prevention, run max. 4 times at same priority */
#define MAX_SAMEPRIO_LENGTH 4


typedef enum TaskState {
    STATE_UNUSED=0,    // Task is not yet used
    STATE_READY,      // The task is ready and should be on a ready queue of the scheduler
    STATE_RUNNING,    // The task is running and should not be on a ready queue
    STATE_WAITING,    // The task is blocked and should not be on a ready queue
    STATE_TERMINATED  // The task has terminated
} TaskState;


/*
 * This datastructure is used to represent tasks
 * For the assignment, the main relavant entyr is priority
 */
typedef struct Task {
	TaskState state;
	int pid;
	int priority;
	char name[8];
} Task;

void initScheduler();

Task *scheduleNextTask();

void onTaskReady(Task *task);
void onTaskPreempted(Task *task);
void onTaskWaiting(Task *task);

#endif
