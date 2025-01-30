#include "testlib.h"
#include "scheduler.h"
#include <stdio.h>

#define test_scheduleNextTask(should) ({ \
    scheduled = scheduleNextTask(); \
    if(scheduled) \
        printf("%s line %d: Next Task: %d (%s)\n", __FILE__, __LINE__, scheduled->pid, scheduled->name); \
    else  \
        printf("%s line %d: Next Task: NULL\n", __FILE__, __LINE__); \
    test_equals_int(scheduled==NULL?-1:scheduled->pid, should); \
})


// state, task ID (pid), priority, name
static Task tasks[] = {
 { STATE_WAITING, 0, 0, "A" },
 { STATE_WAITING, 1, 1, "B" },
 { STATE_WAITING, 2, 3, "C" },
 { STATE_WAITING, 3, 1, "D" },
 { STATE_WAITING, 4, 2, "E" },
 { STATE_WAITING, 5, 1, "F" },
 { STATE_WAITING, 6, 3, "G" },
 { STATE_WAITING, 7, 5, "H" },
};


int main()
{
    Task *scheduled;

    test_start("scheduler.c");
    initScheduler();

    // No task is ready, so scheduler should return NULL
    test_scheduleNextTask(-1); 

    // Task 1 becomes ready, so scheduler should return that task.
    onTaskReady(&tasks[1]);
    test_scheduleNextTask(1);

    // Task 1 is preempted. This means it should be put back in the ready queue
    // As the only task in the queue, it should be scheduled to run again right away
    onTaskPreempted(scheduled);
    test_scheduleNextTask(1);

    // Task 1 is now blocked on some I/O. This means no task is ready,
    // and the scheduler should return -1
    onTaskWaiting(scheduled);
    test_scheduleNextTask(-1);
    // [Note on starvation prevention: Any lower priority tasks had the chance to run here,
    // future decisions regarding starvation prevention will not consider any execution before this

    // Now, task 1 gets the data from the disk, so it should be scheduled again
    onTaskReady(&tasks[1]);
    test_scheduleNextTask(1);
    // [Note on starvation prevetion: Task 1 has run already once now, so after 3 more executions
    //  without giving a lower priority task the chance to run is time to apply the starvation prevention rule]

    // Task 0 and 7 get ready as well...
    onTaskReady(&tasks[0]);
    onTaskReady(&tasks[7]);

    // Alright, so let's test the startvation prevention rule...
    int k = 0;
    do {
        // Task 1, which is still running, gets preempted.
        // Now three tasks are ready T0(prio 0), T1(prio 1), and T7(prio 5)
        // The scheduler should select the highest priority task....
    	// Task 0 should be scheduled for a total of 4 times.
    	for (int i = 0; i < 4; i++) {
       		onTaskPreempted(scheduled);
        	test_scheduleNextTask(0);
    	}
    	// Now we have scheduled Task 0 four times.
    	// This means that a lower priority task should be scheduled next.
	// This should continue until T1 has executed 4 times as well.
        //
    	onTaskPreempted(scheduled);
        if(k < 3) {
    		test_scheduleNextTask(1);
	} else {
    		test_scheduleNextTask(7);
	}
        k++;
    }
    while(k < 4);

    return test_end();
}
