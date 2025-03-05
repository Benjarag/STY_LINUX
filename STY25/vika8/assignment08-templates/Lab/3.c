// c.  Write a C program for Linux that creates two child processes, ls and less, and uses an
//     ordinary pipe to redirect the standard output of ls to the standard input of less.
//     Notes:
//         • Reading input / writing output in Linux is done by reading from/writing to a file
//           descriptor – which is a simple number identifying a file-like entity. By default, each
//           process you create has initially three open file descriptors: 0 (standard input), 1
//           (standard output) and 2 (standard error).
//         • You can use low-level functions (read, write, ..) to access those file descriptors, or
//           high-level library functions (printf, scanf, etc.) that internally will call those low-level
//           functions.
//         • For the task of redirecting, you need to use the dup2 or dup3 system call – consult
//           the corresponding man page!

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pipefd[2]; // pipefd in this case means pipe as in pipe and fd as in file descriptor [2] means 2 file descriptors

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(1);
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid1 == 0) {
        // Child 1 (ls)
        close(pipefd[0]); // Close unused read end of the pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
        close(pipefd[1]); // Close the original write end of the pipe
        execlp("ls", "ls", NULL); // Execute ls
        perror("execlp ls failed"); // If execlp fails
        exit(1); 
    }

    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid2 == 0) {
        // Child 2 (less)
        close(pipefd[1]); // Close unused write end of the pipe
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to the read end of the pipe
        close(pipefd[0]); // Close the original read end of the pipe
        execlp("less", "less", NULL); // Execute less
        perror("execlp less failed"); // If execlp fails
        exit(1);
    }

    // Parent process
    close(pipefd[0]); // Close the read end of the pipe
    close(pipefd[1]); // Close the write end of the pipe
    waitpid(pid1, NULL, 0); // Wait for child 1 to finish
    waitpid(pid2, NULL, 0); // Wait for child 2 to finish

    return 0;
}


// Output:
// $ gcc 3.c -o 3
// $ ./3
// 3
// 3.c
// 3.o
// a.out
// 3
// 3.c
// 3.o

// The output of ls is redirected to less. The output of ls is displayed in less.
