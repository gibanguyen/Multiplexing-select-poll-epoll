#include <sys/epoll.h>
#include <unistd.h>

int main() {
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        return 1;
    }

    struct epoll_event event;
    event.data.fd = STDIN_FILENO;  // monitor standard input
    event.events = EPOLLIN | EPOLLET; // level-triggered and edge-triggered events

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event) == -1) {
        perror("epoll_ctl");
        return 1;
    }

    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];
    while (1) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                printf("Input is available now.\n");
                char buf[1024];
                int num_bytes = read(STDIN_FILENO, buf, sizeof(buf));
                if (num_bytes == -1) {
                    perror("read");
                    break;
                } else if (num_bytes == 0) {
                    printf("End of input stream.\n");
                    break;
                } else {
                    printf("Read %d bytes: %s\n", num_bytes, buf);
                }
            }
        }
    }

    close(epoll_fd);

    return 0;
}