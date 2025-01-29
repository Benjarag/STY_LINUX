#ifndef SCHEDULER_H
#define SCHEDULER_H

/*
 * The maximum priority.
 * Note: 0 is highest priorty, MAX_PRIORITY is the lowest priority
 */
#define MAX_PRIORITY 10 



typedef enum TaskState {
    STATE_NEW=0,      // Task is not yet used
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
