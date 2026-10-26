#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO_PATH "./q37_fifo"

int main(void) {
    if (mkfifo(FIFO_PATH, 0600) == -1 && errno != EEXIST) { perror("mkfifo"); return 1; }
    printf("Reader waiting for writer on %s ...\n", FIFO_PATH); fflush(stdout);
    int fd = open(FIFO_PATH, O_RDONLY); /* Blocks until a writer opens the other end. */
    if (fd == -1) { perror("open FIFO"); return 1; }
    char buf[256]; ssize_t n;
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        if (fwrite(buf, 1, (size_t)n, stdout) != (size_t)n) { perror("fwrite"); close(fd); return 1; }
    }
    if (n == -1) perror("read FIFO");
    close(fd);
    return n == -1 ? 1 : 0;
}
