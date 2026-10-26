#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static int write_all(int fd, const char *s, size_t len) {
    while (len) {
        ssize_t n = write(fd, s, len);
        if (n == -1) return -1;
        s += n; len -= (size_t)n;
    }
    return 0;
}

int main(void) {
    int parent_to_child[2], child_to_parent[2];
    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        perror("pipe"); return EXIT_FAILURE;
    }
    pid_t pid = fork();
    if (pid == -1) { perror("fork"); return EXIT_FAILURE; }

    if (pid == 0) {
        close(parent_to_child[1]); close(child_to_parent[0]);
        char input[128];
        ssize_t n = read(parent_to_child[0], input, sizeof(input) - 1);
        if (n == -1) { perror("child read"); _exit(EXIT_FAILURE); }
        input[n] = '\0';
        printf("Child received: %s\n", input);
        fflush(stdout);
        const char reply[] = "Reply from child";
        if (write_all(child_to_parent[1], reply, sizeof(reply)) == -1) perror("child write");
        close(parent_to_child[0]); close(child_to_parent[1]);
        _exit(EXIT_SUCCESS);
    }

    close(parent_to_child[0]); close(child_to_parent[1]);
    const char message[] = "Hello child";
    if (write_all(parent_to_child[1], message, sizeof(message)) == -1) perror("parent write");
    close(parent_to_child[1]);
    char reply[128];
    ssize_t n = read(child_to_parent[0], reply, sizeof(reply) - 1);
    if (n == -1) perror("parent read");
    else { reply[n] = '\0'; printf("Parent received: %s\n", reply); }
    close(child_to_parent[0]);
    int status; waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
}
