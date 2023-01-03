#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h> // gettimeofday()
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define ICMP_HDRLEN 8
#define IP "127.0.0.1"
#define port 3000

int main()
{
    // Create raw socket for IP-RAW (make IP-header by yourself)
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


    char buffer[ICMP_HDRLEN + BUFFER_SIZE];

    int counter = 1;

    float timer = 0.0;
    
    struct timeval start, end;

    printf("im here\n");

    while (timer < 10)
    {
        printf("(%d)\n", counter);
        gettimeofday(&start,0);

        recv(sock,buffer,BUFFER_SIZE,0);

        // printf("(%d)\n", counter);

        gettimeofday(&end,0);

        timer = (double)(end.tv_sec - start.tv_sec);
        counter++;
    }
    
    printf("sending timeout\n");
    send(sock,"timeout",8,0);

    return 0;
}
