#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    int fd[2];
    char buffer[128];
    const char message[] = "Hello through a pipe!";

    if (pipe(fd) == -1) { perror("pipe"); return EXIT_FAILURE; }
    printf("Pipe created: read fd=%d, write fd=%d\n", fd[0], fd[1]);

    if (write(fd[1], message, sizeof(message)) == -1) {
        perror("write"); close(fd[0]); close(fd[1]); return EXIT_FAILURE;
    }
    close(fd[1]); /* No more writes; a reader can eventually observe EOF. */

    ssize_t n = read(fd[0], buffer, sizeof(buffer) - 1);
    if (n == -1) { perror("read"); close(fd[0]); return EXIT_FAILURE; }
    buffer[n] = '\0';
    printf("Read from pipe: %s\n", buffer);
    close(fd[0]);
    return EXIT_SUCCESS;
}
