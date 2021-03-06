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

    int cNum = 0, nNum = 0, pNum = 0;
    int rNum;

    char* rPort;

    int recvP;

    int recvFd;
    struct addrinfo* recvInfo;
    struct sockaddr_storage recvAddr;
    socklen_t recvLen;

    if (argc != 3) {
        printf("usage: ./receiver-a <receiver port> <probability>\n");
        exit(1);
    }

    /* Collect and validate arguments */
    rPort = argv[1];
    recvP = atoi(argv[2]);
    if (!check_port(rPort)) {
        printf("receiver-a: port number must be between 30000 and 40000\n");
        exit(1);
    }
    if (recvP < PROB_MIN || recvP > PROB_MAX) {
        printf("receiver-a: probability must be between %d and %d\n", PROB_MIN, PROB_MAX);
        exit(1); 
    }

    /* Establish connection */
    recvFd = udp_socket(&recvInfo, NULL, rPort);
    if (recvFd <= 0) {
        printf("receiver-a: failed to create udp socket for given receiver\n");
        exit(1);
    }
    if (bind(recvFd, recvInfo->ai_addr, recvInfo->ai_addrlen) == -1) {
        printf("receiver-a: failed to bind udp socket for given receiver\n");
        exit(1);
    }

    /* Claim necessary memory */
    input = calloc(MSG_SIZE - 1, sizeof(char));
    message = calloc(MSG_SIZE, sizeof(char));
    if (input == NULL || message == NULL) {
        printf("receiver-a: failed to allocate necessary memory\n");
        exit(1);
    }

    /* Interact with the user */
    while (1) {
        /* Handle incoming message */
        recvLen = sizeof(recvAddr);
        memset(message, 0, MSG_SIZE);
        if (recvfrom(recvFd, message, MSG_SIZE, 0, (struct sockaddr*) &recvAddr, &recvLen) == -1) {
            printf("receiver-a: failed to receive message\n");
        }

        /* Handle user input */
        printf("receiver-a: received message? (Y/N)\n");
        memset(input, 0, MSG_SIZE - 1);
        read(STD_IN, input, MSG_SIZE - 1);

        /* Message successfully received */
        if (input[0] == 'Y') {
            /* Respond to given message */
            cNum = (int) message[0];
            if (cNum == nNum) {
                printf("receiver-a: expected message %d - %s", cNum, message + 1);

                pNum = nNum;
                nNum = (nNum + 1) % SEQ_MAX;
            } else if (cNum == pNum) {
                printf("receiver-a: retransmitted message %d - %s", cNum, message + 1);
            } else {
                printf("receiver-a: unexpected message %d - %s", cNum, message + 1);

                continue;
            }

            /* Attempt to acknowledge message */
            rNum = (rand() % PROB_MAX) + 1;
            if (rNum <= recvP) {
                printf("receiver-a: acknowledgement for %d successful\n", cNum);

                memset(message + 1, 0, MSG_SIZE - 1);
                sprintf(message + 1, "%s", "ack");
                if (sendto(recvFd, message, MSG_SIZE, 0, (struct sockaddr*) &recvAddr, recvLen) == -1) {
                    printf("receiver-a: failed to send message\n");
                }
            } else {
                printf("receiver-a: acknowledgement for %d unsuccessful\n", cNum);
            }
        }
        /* Message corrupted or lost */ 
        else {
            printf("receiver-a: message not successfully received\n");
        }
    }

    close(recvFd);
    exit(0);
}
