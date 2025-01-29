#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "scheduler.h"

/*
 * Put your list implementation here (without the definition on struct Task,
 * this structure is now defined in scheduler.h
 */

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
}

void onTaskReady(Task *task) {
    // TODO: implement
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
