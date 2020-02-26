#include "lib/common.h"
#define MESSAGE_SIZE 1024000

void send_data(int sockfd) {
    char * query; 
    query = malloc(MESSAGE_SIZE + 1);
    for(int i = 0; i < MESSAGE_SIZE; i++) {
        query[i] = 'b';
    }
    query[MESSAGE_SIZE] = '\0';

    const char *cp; 
    cp = query; 
    size_t remaining = strlen(query);
    while(remaining) {
        int n_written = send(sockfd, cp, remaining, 0);
        fprintf(stdout, "send into buffer %ld \n", n_written);
        if(n_written <= 0) {
            error(1, errno, "send failed");
            return; 
        }

        remaining -= n_written; 
        cp += n_written; 
    }
}

int main(int argc, char* argv[]) {
    int sockfd; 
    struct sockaddr_in seraddr; 
    if(argc != 2) {
        error(1, 0, "usage: tcpclient <IPAddress");
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET; 
    seraddr.sin_port = htons(12345);
    inet_pton(AF_INET, argv[1], &seraddr.sin_addr);
    int connect_rt = connect(sockfd, (struct sockaddr*)&seraddr, sizeof(seraddr));
    if(connect_rt < 0) {
        error(1, errno, "connect failed ");
    }

    send_data(sockfd);
    
    return 0; 
}