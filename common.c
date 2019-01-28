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

int unblock_fd(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        return -1;
    }
    
    return flags;
}


int block_fd(int fd, int flags) {
    return fcntl(fd, F_SETFL, flags);
}

int udp_socket(struct addrinfo** outInfo, char* mName, char* port) {
    struct addrinfo* pAi;
    struct addrinfo hints;
    int sockFd;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
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
