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
    char* input;
    char* message;

    int sNum;

    char* rPort;

    int wSize;
    int recvP;

    int recvFd;
    struct addrinfo* recvInfo;
    struct sockaddr_storage recvAddr;
    socklen_t recvLen;

    if (argc != 4) {
        printf("usage: ./receiver-a <port number> <window size> <probability>\n");
        exit(1);
    }

    /* Arguments and connect */
    rPort = argv[1];
    wSize = atoi(argv[2]);
    recvP = atoi(argv[3]);
    if (!check_port(rPort)) {
        printf("receiver-a: port number must be between 30000 and 40000\n");
        exit(1);
    }
    if (wSize < WSIZE_MIN || wSize > WSIZE_MAX) {
        printf("receiver-a: window size must be between %d and %d\n", WSIZE_MAX, WSIZE_MIN);
        exit(1); 
    }
    if (recvP < PROB_MIN || recvP > PROB_MAX) {
        printf("receiver-a: probability must be between %d and %d\n", PROB_MIN, PROB_MAX);
        exit(1); 
    }

    recvFd = udp_socket(&recvInfo, NULL, rPort);
    if (recvFd <= 0) {
        printf("receiver-a: failed to create udp socket for given receiver\n");
        exit(1);
    }
    if (bind(recvFd, recvInfo->ai_addr, recvInfo->ai_addrlen) == -1) {
        printf("receiver-a: failed to bind udp socket for given receiver\n");
        exit(1);
    }

    /* Setup the interactions */
    input = calloc(MSG_SIZE - 1, sizeof(char));
    message = calloc(MSG_SIZE, sizeof(char));
    if (message == NULL) {
        printf("receiver-a: failed to allocate necessary memory\n");
        exit(1);
    }

    /* Interact with the user */
    sNum = 0;
    while (1) {
        printf("receiving message\n");
        if (recvfrom(recvFd, message, MSG_SIZE, 0, (struct sockaddr*) &recvAddr, &recvLen) == -1) {
            printf("Failed to receive message\n");
            printf("%d - %s\n", errno, strerror(errno));
        }
        printf("message received\n");

        printf("receiver-a: reveived message? (Y/N) ");
        fgets(input, MSG_SIZE - 1, stdin);
        if (strcmp(input, "Y") == 0) {
            sNum = (int) message[0];
            printf("receiver-a: received message %d - %s\n", sNum, message + 1);
            memset(message + 1, 0, MSG_SIZE - 1);

            sprintf(message + 1, "%s", "ack");
            if (sendto(recvFd, message, MSG_SIZE, 0, (struct sockaddr*) &recvAddr, recvLen) == -1) {
                printf("Failed to send message\n");
                printf("%d - %s\n", errno, strerror(errno));
            }
        }
    }

    close(recvFd);
    exit(0);
}
