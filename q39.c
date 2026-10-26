#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv) {
    const char *path = argc > 1 ? argv[1] : "./q39_fifo";
    if (mkfifo(path, 0600) == -1 && errno != EEXIST) { perror("mkfifo"); return 1; }
    /* Linux permits O_RDWR on a FIFO. Holding a write end avoids immediate EOF readiness. */
    int fd = open(path, O_RDWR | O_NONBLOCK);
    if (fd == -1) { perror("open FIFO"); return 1; }
    printf("Waiting up to 10 seconds for data on %s ...\n", path); fflush(stdout);
    fd_set set; FD_ZERO(&set); FD_SET(fd, &set);
    struct timeval timeout = { .tv_sec = 10, .tv_usec = 0 };
    int rc = select(fd + 1, &set, NULL, NULL, &timeout);
    if (rc == -1) { perror("select"); close(fd); return 1; }
    if (rc == 0) { puts("Timeout: no data arrived within 10 seconds."); close(fd); return 0; }
    char buf[512]; ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n == -1) { if (errno == EAGAIN) puts("FIFO became ready, but no data was available."); else perror("read"); }
    else { buf[n] = '\0'; printf("Received: %s\n", buf); }
    close(fd);
    return n == -1 && errno != EAGAIN ? 1 : 0;
}
