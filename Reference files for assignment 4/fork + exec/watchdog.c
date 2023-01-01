#include <stdio.h>
#include <time.h>
#include <sys/time.h> // gettimeofday()


#define TIMEOUT_SECONDS 10

int main()
{
    printf("Hello partb\n");
    time_t start_time = time(NULL);
    while (difftime(time(NULL), start_time) < TIMEOUT_SECONDS)
    {
        recv();
        start_time = time(NULL);
    }
    send("timeout");

    return 0;
}
