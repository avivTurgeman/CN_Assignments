#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define IP "127.0.0.1"
#define port 3000

int main()
{
    //initializing a TCP socket.
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        printf("Could not create socket.\n");
        return -1;
    }

    struct sockaddr_in receiver_adderess;
    //setting to zero the struct senderAddress
    memset(&receiver_adderess,0,sizeof(receiver_adderess));
    receiver_adderess.sin_family = AF_INET;
    receiver_adderess.sin_port = htons(port);
    int checkP = inet_pton(AF_INET,(const char*)IP,&receiver_adderess.sin_addr);
    
    if(checkP < 0){
        printf("inet_pton() FAILED.\n");
        return -1;
    }
    
    //connecting to the Receiver on the socket
    int connectCheck = connect(sock,(struct sockaddr*) &receiver_adderess,sizeof(receiver_adderess));
    
    if(connectCheck == -1){
        printf("connect() FAILED.\n");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    float timer = 0.0;
    struct timeval start, end;
    int byets = 0;
    int start_time = 0;
    while (timer < 10)
    {
        if(start_time == 0) {
            gettimeofday(&start, 0);
            start_time = 1;
        }

        byets = recv(sock,buffer,BUFFER_SIZE,MSG_DONTWAIT);

        if(byets > 0) {
            byets = 0;
            start_time = 0;
            timer = 0.0;
        }
        else {
            gettimeofday(&end, 0);
            timer = (double) (end.tv_sec - start.tv_sec);
        }
    }
    
    printf("WATCHDOG:sending timeout\n");
    send(sock,"timeout",8,0);
    close(sock);

    return 0;
}
