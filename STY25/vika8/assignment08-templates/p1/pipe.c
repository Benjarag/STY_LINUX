// This changes the way some includes behave.
// This should stay before any include.
#define _GNU_SOURCE

#include "pipe.h"
#include <sys/wait.h> /* For waitpid */
#include <unistd.h> /* For fork, pipe */
#include <stdlib.h> /* For exit */
#include <fcntl.h>
#include <errno.h>

int execute(char *argv[])
{

    if (argv == NULL) {
        return -1;
    }

    // -------------------------
    // TODO: Open a pipe
    // -------------------------
    int pipefd[2]; // 0 is read end, 1 is write end

    // creating a pape with O_CLOEXEC flag to prevent FD leakage after exec
    if (pipe2(pipefd, O_CLOEXEC) == -1) { // O_CLOEXEC is a flag that closes the pipe when the process is replaced
        return -1; // error
    } // pipe 2 is used to set the flags

    int child_pid = fork();
    if (child_pid == -1) {
        return -1;
    } else if (child_pid == 0) {

        // close the read end of the pipe
        close(pipefd[0]);

        // Replace program
        execvp(argv[0], argv);

        // -------------------------
        // TODO: Write the error on the pipe
        // -------------------------

        int exec_error = errno; // save the error code if execvp fails

        // writing the error code to the pipe
        write(pipefd[1], &exec_error, sizeof(exec_error));

        // close the write end of the pipe
        close(pipefd[1]);
        exit(0);
    } else {
        int status, hadError = 0;

        // close the write end of the pipe
        close(pipefd[1]);

        int waitError = waitpid(child_pid, &status, 0);
        if (waitError == -1) {
            // Error while waiting for child.
            hadError = 1;
        } else if (!WIFEXITED(status)) {
            // Our child exited with another problem (e.g., a segmentation fault)
            // We use the error code ECANCELED to signal this.
            hadError = 1;
            errno = ECANCELED;
        } else {
            // -------------------------
            // TODO: If there was an execvp error in the child, set errno
            //       to the value execvp set it to.
            // -------------------------
            int exec_error = 0; // initializing a variable to hold error code

            ssize_t error_read = read(pipefd[0], &exec_error, sizeof(exec_error));
            if (error_read > 0) { // if we read data from the pipe
                hadError = 1; // mark that we had error
                errno = exec_error; // setting the parent errno to match the childs error
            } else if (error_read == -1) { // if there was an error reading from the pipe
                hadError = 1;
                errno = ECANCELED; // set errno to ECANCELED
            }

        }
        // Close the read end of the pipe
        close(pipefd[0]);

        return hadError ? -1 : WEXITSTATUS(status);
    }
}
