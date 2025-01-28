#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>


int main()
{
    pid_t pid;
    int retval;

    switch( (pid = fork()) )
    {
    case -1:
        printf( "Error. Fork failed\n" );
        break;
    case 0:
        printf("I am the child!\n");
        execl(NULL, NULL, NULL);
        perror("exec failed");
        return -1;  // <-- Change this line
        // return 42;
        // while(1);

	// alternatively put a while(1); here and kill the child in a second terminal

    default: // pid > 0
        printf( "I am the parent!\n" \
                "Child PID is %d\n", pid );
        wait( &retval );
        printf( "Child terminated with exit status %d (%x)\n", retval, retval );
    }

    return 0;
}
