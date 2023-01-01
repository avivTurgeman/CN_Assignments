#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// run 2 programs using fork + exec
// command: make clean && make all && ./partb
int main()
{
    char *args[2];
    // compiled watchdog.c by makefile
    args[0] = "./watchdog";
    args[1] = NULL;
    int status;
    int pid = fork();
    if (pid == 0)
    {
        printf("in child \n");
        execvp(args[0], args);
    }
    wait(&status); // waiting for child to finish before exiting
    printf("child exit status is: %d", status);
    return 0;
}
