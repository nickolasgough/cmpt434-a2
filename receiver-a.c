/* Nickolas Gough, nvg081, 11181823 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#include "common.h"


int main(int argc, char* argv[]) {
    char* message;
    int sNum;
    char* rPort;
    int wSize;
    int recvFd;
    struct addrinfo* recvInfo;
    struct sockaddr_storage recvAddr;
    socklen_t recvLen;
    int inFlags;
    int rFlags;

    if (argc != 3) {
        printf("usage: ./receiver-a <port number> <window size>\n");
        exit(1);
    }

    /* Arguments and connect */
    rPort = argv[1];
    wSize = atoi(argv[2]);
    if (!check_port(rPort)) {
        printf("receiver-a: port number must be between 30000 and 40000\n");
        exit(1);
    }
    if (wSize < WSIZE_MIN || wSize > WSIZE_MAX) {
        printf("receiver-a: window size must be between %d and %d\n", WSIZE_MAX, WSIZE_MIN);
        exit(1); 
    }
    if (!udp_socket(&recvFd, &recvInfo, NULL, rPort)) {
        printf("receiver-a: failed to create udp socket for given receiver\n");
        exit(1);
    }
    if (bind(recvFd, recvInfo->ai_addr, recvInfo->ai_addrlen) == -1) {
        printf("receiver-a: failed to bind udp socket for given receiver\n");
        exit(1);
    }

    /* Setup the interactions */
    message = calloc(MAX_SIZE, sizeof(char));
    if (message == NULL) {
        printf("receiver-a: failed to allocate necessary memory\n");
        exit(1);
    }
    // inFlags = unblock_fd(STD_IN);
    // rFlags = unblock_fd(recvFd);

    recvfrom(recvFd, message, MAX_SIZE, 0, (struct sockaddr*) &recvAddr, &recvLen);
    printf("%s\n", message);

    if (sendto(recvFd, "hello", MAX_SIZE, 0, (struct sockaddr*) &recvAddr, recvLen) == -1) {
        printf("Failed to send message\n");
        printf("%d\n", errno);
        printf("%s\n", strerror(errno));
        exit(1);
    }
    exit(0);

    /* Interact with the user */
    // bCount = 0;
    // sNum = 0;
    // while (1) {
    //     printf("receiver-a? ");

    //     message = calloc(MAX_SIZE, sizeof(char));
    //     if (message == NULL) {
    //         printf("receiver-a: failed to allocate necessary memory\n");
    //         exit(1);
    //     }

    //     while (1) {
    //         if (read(STD_IN, input, MAX_SIZE-1) != -1) {
    //             message[0] = (char) sNum;
    //             sprintf(&message[1], "%s", input);
    //             buffer[sNum] = message;

    //             sNum += 1;
    //             sNum = sNum % (wSize+1);
    //             break;
    //         }
    //     }
    // }

    // block_fd(STD_IN, inFlags);

    close(recvFd);
    exit(0);
}
