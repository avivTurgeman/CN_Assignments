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
#define DESTINATION_IP "8.8.8.8"

int main()
{
    // Create raw socket for IP-RAW (make IP-header by yourself)
    int sock = -1;
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        fprintf(stderr, "socket() failed with error: %d", errno);
        fprintf(stderr, "To create a raw socket, the process needs to be run by Admin/root user.\n\n");
        return -1;
    }

    char buffer[ICMP_HDRLEN + BUFFER_SIZE];


    float timer = 0.0;
    
    struct timeval start, end;

    while (timer < 10)
    {
        gettimeofday(&start,0);

        recv(sock,buffer,BUFFER_SIZE,0);

        gettimeofday(&end,0);

        timer = (double)(end.tv_sec - start.tv_sec);
        
    }
    struct sockaddr_in dest_in;
    memset(&dest_in, 0, sizeof(struct sockaddr_in));
    dest_in.sin_family = AF_INET;

    // The port is irrelant for Networking and therefore was zeroed.
    // dest_in.sin_addr.s_addr = iphdr.ip_dst.s_addr;
    dest_in.sin_addr.s_addr = inet_addr(DESTINATION_IP);
    // inet_pton(AF_INET, DESTINATION_IP, &(dest_in.sin_addr.s_addr));
    
    printf("sending timeout\n");
    sendto(sock,"timeout",8,0, (struct sockaddr *)&dest_in, sizeof(dest_in));

    return 0;
}
