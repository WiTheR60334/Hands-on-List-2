#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_PATH "./q37_fifo"

int main(int argc, char **argv) {
    const char *message = argc > 1 ? argv[1] : "Hello through a FIFO!\n";
    if (mkfifo(FIFO_PATH, 0600) == -1 && errno != EEXIST) { perror("mkfifo"); return 1; }
    int fd = open(FIFO_PATH, O_WRONLY); /* Blocks until a reader opens the other end. */
    if (fd == -1) { perror("open FIFO"); return 1; }
    size_t left = strlen(message); const char *p = message;
    while (left) {
        ssize_t n = write(fd, p, left);
        if (n == -1) { perror("write FIFO"); close(fd); return 1; }
        p += n; left -= (size_t)n;
    }
    close(fd); /* Reader sees EOF after every writer closes its descriptor. */
    return 0;
}
