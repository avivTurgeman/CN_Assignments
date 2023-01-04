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
#include <unistd.h>
#include <sys/wait.h>

// IPv4 header len without options
#define IP4_HDRLEN 20
// ICMP header len for echo req
#define ICMP_HDRLEN 8
// IP of Watchdog
#define WATCHDOG_IP "127.0.0.1"
//exit message
#define EXIT_MESSAGE "timeout"
#define port 3000

// Checksum algo
unsigned short calculate_checksum(unsigned short *paddress, int len);
int isValidIp4 (char *str);

int main(int argc,char *argv[]) {

    if (argc != 2) {
        printf("invalid input!\n");
        return -1;
    }
    if (isValidIp4(argv[1]) != 1) {
        printf("invalid IP!\n");
        return -1;
    }
    char *destenation_ip = argv[1];
    printf("dest ip is: %s\n", destenation_ip);

    char *args[2];
    // compiled watchdog.c by makefile
    args[0] = "./watchdog";
    args[1] = NULL;
    int status;
    int pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
    }
    struct icmp icmphdr; // ICMP-header
    char data[IP_MAXPACKET] = "This is aviv's & alon's ping test.\n";
    int datalen = (int) strlen(data) + 1;

    int sequance = 1;

    int sock = -1;
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        fprintf(stderr, "socket() failed with error: %d\n", errno);
        fprintf(stderr, "To create a raw socket, the process needs to be run by Admin/root user.\n\n");
        return -1;
    }

    //initializing a TCP socket.
    int TCPsock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in senderAddress;

    //setting to zero the struct senderAddress
    memset(&senderAddress, 0, sizeof(senderAddress));
    senderAddress.sin_family = AF_INET;
    senderAddress.sin_port = htons(port);
    int checkP = inet_pton(AF_INET, (const char *) WATCHDOG_IP, &senderAddress.sin_addr);
    if (checkP <= 0) {
        printf("inet_pton() failed.\n");
        return -1;
    }

    //opening the socket.
    int Bcheck = bind(TCPsock, (struct sockaddr *) &senderAddress, sizeof(senderAddress));
    if (Bcheck == -1) {
        fprintf(stderr, "bind failed: %d\n", errno);
        close(TCPsock);
        return -1;
    }

    //start listening on the socket (one client at the time)
    int Lcheck = listen(TCPsock, 1);
    if (Lcheck == -1) {
        printf("Error in listen().\n");
        return -1;
    }

    //accepting the client (the Sender)
    unsigned int senderAddressLen = sizeof(senderAddress);
    int newTCPsock = accept(TCPsock, (struct sockaddr *) &senderAddress, &senderAddressLen);
    if (newTCPsock == -1) {
        printf("accept() failed.\n");
        close(newTCPsock);
        return -1;
    }
    while(1){
        //===================
        // ICMP header
        //===================

        // Message Type (8 bits): ICMP_ECHO_REQUEST
        icmphdr.icmp_type = ICMP_ECHO;

        // Message Code (8 bits): echo request
        icmphdr.icmp_code = 0;

        // Identifier (16 bits): some number to trace the response.
        // It will be copied to the response packet and used to map response to the request sent earlier.
        // Thus, it serves as a Transaction-ID when we need to make "ping"
        icmphdr.icmp_id = 18;

        // Sequence Number (16 bits): starts at 0
        icmphdr.icmp_seq = 0;

        // ICMP header checksum (16 bits): set to 0 not to include into checksum calculation
        icmphdr.icmp_cksum = 0;

        // Combine the packet
        char packet[IP_MAXPACKET];

        // Next, ICMP header
        memcpy((packet), &icmphdr, ICMP_HDRLEN);

        // After ICMP header, add the ICMP data.
        memcpy(packet + ICMP_HDRLEN, data, datalen);

        // Calculate the ICMP header checksum
        icmphdr.icmp_cksum = calculate_checksum((unsigned short *)(packet), ICMP_HDRLEN + datalen);
        memcpy((packet), &icmphdr, ICMP_HDRLEN);

        struct sockaddr_in dest_in;
        memset(&dest_in, 0, sizeof(struct sockaddr_in));
        dest_in.sin_family = AF_INET;

        // The port is irrelant for Networking and therefore was zeroed.
        // dest_in.sin_addr.s_addr = iphdr.ip_dst.s_addr;
        dest_in.sin_addr.s_addr = inet_addr(destenation_ip);
        // inet_pton(AF_INET, DESTINATION_IP, &(dest_in.sin_addr.s_addr));

        struct timeval start, end;
        gettimeofday(&start, 0);
        // Send the ping using sendto() for sending datagrams.
        int bytes_sent = sendto(sock, packet, ICMP_HDRLEN + datalen, 0, (struct sockaddr *)&dest_in, sizeof(dest_in));
        if (bytes_sent == -1)
        {
            fprintf(stderr, "sendto() failed with error: %d\n", errno);
            return -1;
        }

        int ICMPbytes = -1;

        // Get the ping response
        bzero(packet, IP_MAXPACKET);
        socklen_t len = sizeof(dest_in);
        char watchdog_buffer[10] = {'0'};
        int tcp_bytes = 0;

        while(1)
        {
            tcp_bytes = recv(newTCPsock , watchdog_buffer, 10, MSG_DONTWAIT);
            if(tcp_bytes > 0) {
                if (strcmp(packet, EXIT_MESSAGE) == 0) {
                    printf("got Exit message!\n");
                    break;
                }
            }
            ICMPbytes = (int) recvfrom(sock, packet, sizeof(packet), MSG_DONTWAIT, (struct sockaddr *)&dest_in, &len);
            if (ICMPbytes > 0)
            {
                break;
            }
        }

        if(strcmp(watchdog_buffer,EXIT_MESSAGE) == 0){
            break;
        }
        gettimeofday(&end, 0);
        // Send the notification to the watchdog.
        int bytes_sent_to_watchdog = (int) send(newTCPsock, "Got Pong!",sizeof ("Got Pong!"), 0);
        if (bytes_sent_to_watchdog == -1)
        {
            fprintf(stderr, "send() failed with error: %d\n", errno);
            return -1;
        }

        //printing ping pong info
        char reply[IP_MAXPACKET];
        memcpy(reply, packet + ICMP_HDRLEN + IP4_HDRLEN, datalen);
        float milliseconds = (end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f;
        printf("%d bytes from %s: icmp_seq=%d ttl=10 time=%f ms\n", ICMPbytes, argv[1], sequance, milliseconds);

        sleep(1);
        sequance++;
    }
    // Close the raw socket descriptor.
    close(sock);
    close(newTCPsock);
    wait(&status); // waiting for child to finish before exiting
    printf("child exit status is: %d \n", status);
    return 0;
}



int isValidIp4 (char *str) {
    int segs = 0;   /* Segment count. */
    int chcnt = 0;  /* Character count within segment. */
    int accum = 0;  /* Accumulator for segment. */

    /* Catch NULL pointer. */

    if (str == NULL)
        return 0;

    /* Process every character in string. */

    while (*str != '\0') {
        /* Segment changeover. */

        if (*str == '.') {
            /* Must have some digits in segment. */

            if (chcnt == 0)
                return 0;

            /* Limit number of segments. */

            if (++segs == 4)
                return 0;

            /* Reset segment values and restart loop. */

            chcnt = accum = 0;
            str++;
            continue;
        }
        /* Check numeric. */

        if ((*str < '0') || (*str > '9'))
            return 0;

        /* Accumulate and check segment. */

        if ((accum = accum * 10 + *str - '0') > 255)
            return 0;

        /* Advance other segment specific stuff and continue loop. */

        chcnt++;
        str++;
    }
    /* Check enough segments and enough characters in last segment. */

    if (segs != 3)
        return 0;

    if (chcnt == 0)
        return 0;

    /* Address okay. */

    return 1;
}


// Compute checksum (RFC 1071).
unsigned short calculate_checksum(unsigned short *paddress, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = paddress;
    unsigned short answer = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *((unsigned char *)&answer) = *((unsigned char *)w);
        sum += answer;
    }

    // add back carry outs from top 16 bits to low 16 bits
    sum = (sum >> 16) + (sum & 0xffff); // add hi 16 to low 16
    sum += (sum >> 16);                 // add carry
    answer = ~sum;                      // truncate to 16 bits

    return answer;
}