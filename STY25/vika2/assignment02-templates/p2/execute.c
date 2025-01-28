#include "execute.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>


#define ERROR_CODE 127

int execute(char *file_path, char *argv[])
{
    (void) file_path;
    (void) argv;

    if (file_path == NULL) {
        return ERROR_CODE;
    }
    pid_t pid = fork();
    if (pid == -1) {
        return ERROR_CODE; // failiure
    }
    else if (pid == 0) { // child process
        char **new_argv;
        if (argv == NULL) {
            new_argv = malloc(2 * sizeof(char *)); // allocating memory for 2 pointers
            new_argv[0] = file_path; // program name
            new_argv[1] = NULL; // end of parameters
        }
        else {
            int argc = 0;
            while (argv[argc] != NULL) argc++; // count our arguments until NULL
            new_argv = malloc((argc + 2) * sizeof(char *)); // +2 for NULL and file path
            new_argv[0] = file_path;
            for (int i = 0; i < argc; i++) {
                new_argv[i+1] = argv[i]; // user arguments, such as "/"
            }
            new_argv[argc+1] = NULL; // just to be sure that there will be a NULL here
        }
        
        // Execute program searches the path automatically
        execvp(file_path, new_argv);
        
        // if it fails
        free(new_argv);
        exit(ERROR_CODE); // failiure signal to the parent process
    }
    else { // pid > 0, parent process
        int wstatus;
        int wait_result = waitpid(pid, &wstatus, 0); // waiting for child

        if (wait_result == -1) {
            return ERROR_CODE;
        }
        else if (WIFEXITED(wstatus)) { // returns true if the child terminated normally
            int exit_status = WEXITSTATUS(wstatus); // returns the exit status of the child
            if (exit_status == ERROR_CODE) {
                return ERROR_CODE;
            }
            else {
                return exit_status; 
            }
        }
        else {
            return ERROR_CODE;
        }
    }
}
