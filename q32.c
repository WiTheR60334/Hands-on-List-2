#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int fd[2];
    if (pipe(fd) == -1) { perror("pipe"); return EXIT_FAILURE; }

    pid_t pid = fork();
    if (pid == -1) { perror("fork"); close(fd[0]); close(fd[1]); return EXIT_FAILURE; }
    if (pid == 0) {
        close(fd[1]);
        char buffer[256];
        ssize_t n = read(fd[0], buffer, sizeof(buffer) - 1);
        if (n == -1) { perror("child read"); close(fd[0]); _exit(EXIT_FAILURE); }
        buffer[n] = '\0';
        printf("Child received: %s\n", buffer);
        fflush(stdout);
        close(fd[0]);
        _exit(EXIT_SUCCESS);
    }

    close(fd[0]);
    const char message[] = "Data sent by the parent process";
    if (write(fd[1], message, sizeof(message)) == -1) perror("parent write");
    close(fd[1]);
    int status;
    if (waitpid(pid, &status, 0) == -1) { perror("waitpid"); return EXIT_FAILURE; }
    return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
}
