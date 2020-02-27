#include "lib/common.h"


int main(int argc, char* argv[]) {
    // int socked_fd = tcp_server_listen(SERV_PORT);
    int socket_fd = tcp_nonblocking_server_listen(SERV_PORT);
    struct sockaddr_in cliaddr; 
    socklen_t chilen; 
    while(1) {
        printf("Blocking on accept socket...\n");
        int fd = accept(socket_fd, (struct sockaddr*)&cliaddr, &chilen);
        printf("accept fd %d\n", fd);
        usleep(5000000);
    }
}