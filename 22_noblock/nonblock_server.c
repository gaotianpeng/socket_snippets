#include "lib/common.h"
#define MAX_LINE 1024
#define FD_INIT_SIZE 128

char rot13_char(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

struct Buffer {
    int connect_fd; 
    char buffer[MAX_LINE];
    size_t write_index; 
    size_t read_index; 
    int readable; 
};

struct Buffer *alloc_Buffer() {
    struct Buffer *buffer = malloc(sizeof(struct Buffer));
    if (!buffer)
        return NULL;
    buffer->connect_fd = 0;
    buffer->write_index = buffer->read_index = buffer->readable = 0;
    return buffer;
}

void free_buffer(struct Buffer *buffer) {
    free(buffer);
}

int onSocketRead(int fd, struct Buffer* buffer) {
    char buf[1024];
    int i; 
    ssize_t result; 
    while(1) {
        result = recv(fd, buf, sizeof(buf), 0);
        if(result <= 0) {
            break; 
        }

        for(i = 0; i < result; ++i) {
            if(buffer->write_index < sizeof(buffer->buffer)) {
                buffer->buffer[buffer->write_index++] = rot13_char(buf[i]); 
            }

            if(buf[i] == '\n') {
                buffer->readable = 1; 
            }
        }
    }

    if(result == 0) {
        return 1; 
    } else if (result < 0) {
        if (errno == EAGAIN) {
            return 0; 
        }
        return -1; 
    }

    return 0; 
}

int onSocketWrite(int fd, struct Buffer* buffer) {
    while (buffer->read_index < buffer->write_index) {
        ssize_t result = send(fd, buffer->buffer + buffer->read_index, buffer->write_index - buffer->read_index, 0);
        if (result < 0) {
            if (errno == EAGAIN)
                return 0;
            return -1;
        }

        buffer->read_index += result;
    }

    //readindex已经追上writeIndex，说明有效发送区间已经全部读完，将readIndex和writeIndex设置为0，复用这段缓冲
    if (buffer->read_index == buffer->write_index)
        buffer->read_index = buffer->write_index = 0;

    //缓冲数据已经全部读完，不需要再读
    buffer->readable = 0;
    return 0;
}

int main(int argc, char** argv) {
    int listen_fd; 
    int i, maxfd; 
    
    struct Buffer* buffer[FD_INIT_SIZE];
    for(i = 0; i < FD_INIT_SIZE; ++i) {
        buffer[i] = alloc_Buffer();
    }

    listen_fd = tcp_nonblocking_server_listen(SERV_PORT);
    fd_set readset, writeset, exset; 
    FD_ZERO(&readset);
    FD_ZERO(&writeset);
    FD_ZERO(&exset);

    while (1) {
        maxfd = listen_fd;

        FD_ZERO(&readset);
        FD_ZERO(&writeset);
        FD_ZERO(&exset);

        // listener加入readset
        FD_SET(listen_fd, &readset);

        for (i = 0; i < FD_INIT_SIZE; ++i) {
            if (buffer[i]->connect_fd > 0) {
                if (buffer[i]->connect_fd > maxfd)
                    maxfd = buffer[i]->connect_fd;
                FD_SET(buffer[i]->connect_fd, &readset);
                if (buffer[i]->readable) {
                    FD_SET(buffer[i]->connect_fd, &writeset);
                }
            }
        }

        if (select(maxfd + 1, &readset, &writeset, &exset, NULL) < 0) {
            error(1, errno, "select error");
        }

        if (FD_ISSET(listen_fd, &readset)) {
            printf("listening socket readable\n");
            sleep(5);
            struct sockaddr_storage ss;
            socklen_t slen = sizeof(ss);
            int fd = accept(listen_fd, (struct sockaddr *) &ss, &slen);
            if (fd < 0) {
                error(1, errno, "accept failed");
            } else if (fd > FD_INIT_SIZE) {
                error(1, 0, "too many connections");
                close(fd);
            } else {
                make_nonblocking(fd);
                if (buffer[fd]->connect_fd == 0) {
                    buffer[fd]->connect_fd = fd;
                } else {
                    error(1, 0, "too many connections");
                }
            }
        }

        for (i = 0; i < maxfd + 1; ++i) {
            int r = 0;
            if (i == listen_fd)
                continue;

            if (FD_ISSET(i, &readset)) {
                r = onSocketRead(i, buffer[i]);
            }
            if (r == 0 && FD_ISSET(i, &writeset)) {
                r = onSocketWrite(i, buffer[i]);
            }
            if (r) {
                buffer[i]->connect_fd = 0;
                close(i);
            }
        }
    }

    return 0;
}