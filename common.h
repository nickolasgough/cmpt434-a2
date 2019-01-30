/* Nickolas Gough, nvg081, 11181823 */


#define STD_IN 0

#define PORT_MIN 30000
#define PORT_MAX 40000

#define SEQ_MIN 0
#define SEQ_MAX 255

#define WSIZE_MIN 1
#define WSIZE_MAX 255

#define PROB_MIN 1
#define PROB_MAX 100

#define TIME_MIN 1
#define TIME_MAX 60

#define HEAD_SIZE 3
#define MSG_SIZE 1000


int check_port(char* port);

int unblock_fd(int fd);

int block_fd(int fd, int flags);

int udp_socket(struct addrinfo** outInfo, char* mName, char* port);
