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
    int bHead = 0, bCount = 0;

    char* input;
    char* message;

    char* rName;
    char* rPort;

    int cNum = 0, s1Num, s2Num;
    int wSize, tOut;
    struct timeval tv;

    int recvFd;
    struct addrinfo* recvInfo;
    struct sockaddr* recvAddr;
    socklen_t recvLen;

    fd_set fds;
    int sValue;

    int i;

    if (argc != 5) {
        printf("usage: ./sender-b <host IP> <port number> <window size> <timeout>\n");
        exit(1);
    }

    /* Collect and validate arguments */
    rName = argv[1];
    rPort = argv[2];
    wSize = atoi(argv[3]);
    tOut = atoi(argv[4]);
    if (!check_port(rPort)) {
        printf("sender-b: port number must be between 30000 and 40000\n");
        exit(1);
    }
    if (wSize < WSIZE_MIN || wSize > WSIZE_MAX) {
        printf("sender-b: window size must be between %d and %d\n", WSIZE_MAX, WSIZE_MIN);
        exit(1); 
    }
    if (tOut < TIME_MIN || tOut > TIME_MAX) {
        printf("sender-b: timeout must be between %d and %d\n", TIME_MIN, TIME_MAX);
        exit(1);   
    }

    /* Establish connection */
    recvFd = udp_socket(&recvInfo, rName, rPort);
    if (recvFd <= 0) {
        printf("sender-b: failed to create udp socket for given receiver\n");
        exit(1);
    }
    recvAddr = (struct sockaddr*) recvInfo->ai_addr;
    recvLen = recvInfo->ai_addrlen;

    /* Claim necessary memory */
    buffer = calloc(wSize, sizeof(char*));
    input = calloc(MSG_SIZE - 1, sizeof(char));
    if (buffer == NULL || input == NULL) {
        printf("sender-b: failed to allocate necessary memory\n");
        exit(1);
    }

    /* Interact with the user */
    while (1) {
        FD_ZERO(&fds);
        FD_SET(STD_IN, &fds);
        FD_SET(recvFd, &fds);

        tv.tv_sec = tOut;
        tv.tv_usec = 0;
        printf("sender-b: message to send?\n");
        sValue = select(recvFd + 1, &fds, NULL, NULL, &tv);

        /* Handle input or message */
        if (sValue != 0) {
            /* Handle user input */
            if (FD_ISSET(STD_IN, &fds)) {
                if (bCount >= wSize) {
                    printf("sender-b: failed to send due to full buffer\n");

                    read(STD_IN, input, MSG_SIZE - 1);
                    memset(input, 0, MSG_SIZE - 1);
                    continue;
                }

                message = calloc(MSG_SIZE, sizeof(char));
                if (message == NULL) {
                    printf("sender-b: failed to allocate necessary memory\n");
                    exit(1);
                }

                read(STD_IN, input, MSG_SIZE - 1);
                message[0] = (char) cNum;
                sprintf(message + 1, "%s", input);

                i = (bHead + bCount) % wSize;
                buffer[i] = message;
                bCount += 1;

                cNum = (cNum + 1) % SEQ_MAX;
                memset(input, 0, MSG_SIZE - 1);

                if (sendto(recvFd, message, MSG_SIZE, 0, recvAddr, recvLen) == -1) {
                    printf("sender-b: failed to send message\n");
                }
            }
            /* Handle receiver message */
            if (FD_ISSET(recvFd, &fds)) {
                message = calloc(MSG_SIZE, sizeof(char));
                if (message == NULL) {
                    printf("sender-b: failed to allocate necessary memory\n");
                    exit(1);
                }

                if (recvfrom(recvFd, message, MSG_SIZE, 0, NULL, NULL) == -1) {
                    printf("sender-b: failed to receive message\n");
                }
                if (strcmp(message + 1, "ack") != 0) {
                    printf("sender-b: received an unexpected message\n");
                    continue;
                }

                /* Remove acked messages */
                s1Num = (int) message[0];
                while (bCount > 0) {
                    s2Num = (int) buffer[bHead][0];
                    if (s2Num == s1Num) {
                        break;
                    }

                    free(buffer[bHead]);
                    buffer[bHead] = NULL;

                    bHead = (bHead + 1) % wSize;
                    bCount -= 1;
                }
                printf("sender-b: acknowledgement for %d successful\n", s1Num);

                free(message);
            }
        }
        /* Handle a timeout */
        else {
            if (bCount > 0) {
                printf("sender-b: timeout, retransmitting begnning with %d\n", bHead);
            } else {
                printf("sender-b: timeout with empty buffer, doing nothing\n");
                continue;
            }

            /* Retransmit buffered message */
            message = buffer[bHead];
            if (sendto(recvFd, message, MSG_SIZE, 0, recvAddr, recvLen) == -1) {
                printf("sender-b: failed to send message\n");
            }
        }
    }

    close(recvFd);
    free(input);
    exit(0);
}
