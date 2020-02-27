#include "lib/common.h"

void read_data(int sockfd) {
    ssize_t n; 
    char buf[1024];
    int time = 0; 
    for(;;) {
        fprintf(stdout, "block in read\n");
        if ((n = readn(sockfd, buf, 1024)) == 0) {
            return;
        }

        time++;
        fprintf(stdout, "1K read for %d \n", time);
        usleep(5000);
    }
}

int main(int argc, char* argv[]) {
    int listenfd, connfd;
    socklen_t clilen; 
    struct sockaddr_in cliaddr, servaddr; 
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(listenfd, 1024);
    
    for(;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
        if(connfd > 0) {
            printf("connfd is %d\n", connfd);
            read_data(connfd);
        }

        close(connfd);
    }

    return 0; 
}