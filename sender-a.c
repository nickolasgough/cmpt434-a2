/* Nickolas Gough, nvg081, 11181823 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#include "common.h"


int main(int argc, char* argv[]) {
    char** buffer;
    int bHead;
    int bCount;

    char* input;
    char* message;

    char* rName;
    char* rPort;

    int sNum;
    int wSize;
    int tOut;
    struct timeval tv;

    int recvFd;
    struct addrinfo* recvInfo;
    struct sockaddr* recvAddr;
    socklen_t recvLen;

    fd_set fds;
    int sValue;

    if (argc != 5) {
        printf("usage: ./sender-a <host IP> <port number> <window size> <timeout>\n");
        exit(1);
    }

    /* Arguments and connect */
    rName = argv[1];
    rPort = argv[2];
    wSize = atoi(argv[3]);
    tOut = atoi(argv[4]);
    if (!check_port(rPort)) {
        printf("sender-a: port number must be between 30000 and 40000\n");
        exit(1);
    }
    if (wSize < WSIZE_MIN || wSize > WSIZE_MAX) {
        printf("sender-a: window size must be between %d and %d\n", WSIZE_MAX, WSIZE_MIN);
        exit(1); 
    }
    if (tOut < TIME_MIN || tOut > TIME_MAX) {
        printf("sender-a: timeout must be between %d and %d\n", TIME_MIN, TIME_MAX);
        exit(1);   
    }

    recvFd = udp_socket(&recvInfo, rName, rPort);
    if (recvFd <= 0) {
        printf("sender-a: failed to create udp socket for given receiver\n");
        exit(1);
    }

    /* Setup the interactions */
    buffer = calloc(wSize + 1, sizeof(char*));
    input = calloc(MAX_SIZE - HEAD_SIZE, sizeof(char));
    message = calloc(MAX_SIZE, sizeof(char));
    if (buffer == NULL || message == NULL || input == NULL) {
        printf("sender-a: failed to allocate necessary memory\n");
        exit(1);
    }

    FD_ZERO(&fds);
    FD_SET(STD_IN, &fds);
    FD_SET(recvFd, &fds);

    recvAddr = (struct sockaddr*) recvInfo->ai_addr;
    recvLen = recvInfo->ai_addrlen;

    /* Interact with the user */
    bHead = 0;
    bCount = 0;
    while (1) {
        tv.tv_sec = tOut;
        tv.tv_usec = 0;
        sValue = select(recvFd + 1, &fds, NULL, NULL, &tv);

        if (sValue != 0) {
            if (FD_ISSET(STD_IN, &fds)) {
                if (bCount >= wSize) {
                    printf("sender-a: failed to collect input due to full buffer\n");
                    continue;
                }

                read(STD_IN, input, MAX_SIZE - HEAD_SIZE);
                printf("got input %s\n", input);

                sNum = ((bHead + bCount) % wSize);
                itoa(message, sNum, 10);
                sprintf((message + HEAD_SIZE), "%s", input);
                memset(input, 0, MAX_SIZE - 1);

                buffer[sNum] = message;
                bCount += 1;

                printf("sending message %s\n", message);
                sendto(recvFd, message, MAX_SIZE, 0, recvAddr, recvLen);

                message = calloc(MAX_SIZE, sizeof(char));
                if (message == NULL) {
                    printf("sender-a: failed to allocate necessary memory\n");
                    exit(1);
                }
            }
            if (FD_ISSET(recvFd, &fds)) {
                recvfrom(recvFd, message, MAX_SIZE, 0, NULL, NULL);
                sNum = (int) message[0];
                free(buffer[sNum]);
                buffer[sNum] = NULL;

                bCount -= 1;
                bHead += 1;
                bHead = bHead % (wSize + 1);
            }
        } else {
            printf("sender-a: timeout, start retransmitting messages!\n");
        }
    }

    close(recvFd);
    exit(0);
}
