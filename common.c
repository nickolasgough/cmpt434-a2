/* Nickolas Gough, nvg081, 11181823 */


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include "common.h"


int check_port(char* port) {
    if (atoi(port) < PORT_MIN) {
        return 0;
    }
    if (atoi(port) > PORT_MAX) {
        return 0;
    }

    return 1;
}

int valid_seqn(int sNum, int fNum, int bSize) {
    int lNum = (fNum + (SEQ_MAX - bSize - 1)) % SEQ_MAX;
    
    if (lNum < fNum) {
        return sNum >= fNum || sNum < lNum;
    } else {
        return sNum >= fNum && sNum < lNum;
    }
}

int udp_socket(struct addrinfo** outInfo, char* mName, char* port) {
    struct addrinfo* pAi;
    struct addrinfo hints;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;
    
    if (getaddrinfo(mName, port, &hints, &pAi) != 0) {
        return -1;
    }
    if (pAi->ai_socktype != SOCK_DGRAM || pAi->ai_protocol != IPPROTO_UDP) {
        return -1;
    }
    
    *outInfo = pAi;
    return socket(pAi->ai_family, pAi->ai_socktype, pAi->ai_protocol);
}
