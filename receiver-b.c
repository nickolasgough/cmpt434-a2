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
    char** buffer;
    int bHead = 0, bCount = 0;

    char* input;
    char* message;

    int cNum = 0, nNum = 0, pNum = 0;
    int validS, tempP;

    char* rPort;

    int rSize, recvP;

    int recvFd;
    struct addrinfo* recvInfo;
    struct sockaddr_storage recvAddr;
    socklen_t recvLen;

    int i;

    if (argc != 4) {
        printf("usage: ./receiver-b <port number> <buffer size> <probability>\n");
        exit(1);
    }

    /* Collect and vaidate arguments */
    rPort = argv[1];
    rSize = atoi(argv[2]);
    recvP = atoi(argv[3]);
    if (!check_port(rPort)) {
        printf("receiver-b: port number must be between 30000 and 40000\n");
        exit(1);
    }
    if (rSize < WSIZE_MIN || rSize > WSIZE_MAX) {
        printf("receiver-b: buffer size must be between %d and %d\n", WSIZE_MAX, WSIZE_MIN);
        exit(1); 
    }
    if (recvP < PROB_MIN || recvP > PROB_MAX) {
        printf("receiver-b: probability must be between %d and %d\n", PROB_MIN, PROB_MAX);
        exit(1); 
    }

    /* Establish connection */
    recvFd = udp_socket(&recvInfo, NULL, rPort);
    if (recvFd <= 0) {
        printf("receiver-b: failed to create udp socket for given receiver\n");
        exit(1);
    }
    if (bind(recvFd, recvInfo->ai_addr, recvInfo->ai_addrlen) == -1) {
        printf("receiver-b: failed to bind udp socket for given receiver\n");
        exit(1);
    }

    /* Claim necessary memory */
    buffer = calloc(rSize, sizeof(char*));
    input = calloc(MSG_SIZE - 1, sizeof(char));
    message = calloc(MSG_SIZE, sizeof(char));
    if (buffer == NULL || input == NULL || message == NULL) {
        printf("receiver-b: failed to allocate necessary memory\n");
        exit(1);
    }

    /* Interact with the user */
    while (1) {
        /* Handle incoming message */
        recvLen = sizeof(recvAddr);
        memset(message, 0, MSG_SIZE);
        if (recvfrom(recvFd, message, MSG_SIZE, 0, (struct sockaddr*) &recvAddr, &recvLen) == -1) {
            printf("receiver-b: failed to receive message\n");
        }

        /* Handle user input */
        printf("receiver-b: received message? (Y/N)\n");
        memset(input, 0, MSG_SIZE - 1);
        read(STD_IN, input, MSG_SIZE - 1);

        /* Message successfully received */
        if (input[0] == 'Y') {
            /* Respond to given message */
            cNum = (int) message[0];
            if (cNum == nNum) {
                printf("receiver-b: expected message %d - %s", cNum, message + 1);
                memset(message + 1, 0, MSG_SIZE - 1);

                pNum = nNum;
                nNum = (nNum + 1) % SEQ_MAX;

                while (bCount > 0) {
                    cNum = (int) buffer[bHead][0];
                    if (cNum != nNum) {
                        break;
                    }
                    free(message);

                    message = buffer[bHead];
                    buffer[bHead] = NULL;
                    bHead = (bHead + 1) % rSize;
                    bCount -= 1;

                    printf("receiver-b: buffered message %d - %s", cNum, message + 1);

                    pNum = nNum;
                    nNum = (nNum + 1) % SEQ_MAX;
                }
            } else if (cNum == pNum) {
                printf("receiver-b: retransmitted message %d - %s", cNum, message + 1);
            } else {
                printf("receiver-b: unexpected message %d - %s", cNum, message + 1);

                validS = valid_seqn(cNum, nNum, rSize);
                if (validS && bCount < rSize) {
                    i = (bHead + bCount) % rSize;
                    buffer[i] = message;
                    bCount += 1;

                    printf("receiver-b: message bufferd\n");
                } else {
                    free(message);

                    printf("receiver-b: message discarded\n");
                }

                message = calloc(MSG_SIZE, sizeof(char));
                if (message == NULL) {
                    printf("receiver-b: failed to allocate necessary memory\n");
                    exit(1);
                }
            }

            /* Attempt to acknowledge message */
            tempP = (rand() % PROB_MAX) + 1;
            if (tempP <= recvP) {
                printf("receiver-b: acknowledgement for %d successful\n", nNum);

                message[0] = (char) nNum;
                memset(message + 1, 0, MSG_SIZE - 1);
                sprintf(message + 1, "%s", "ack");
                if (sendto(recvFd, message, MSG_SIZE, 0, (struct sockaddr*) &recvAddr, recvLen) == -1) {
                    printf("receiver-b: failed to send message\n");
                }
            } else {
                printf("receiver-b: acknowledgement for %d unsuccessful\n", nNum);
            }
        }
        /* Message corrupted or lost */ 
        else {
            printf("receiver-b: message not successfully received\n");
        }
    }

    close(recvFd);
    exit(0);
}
