#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int p[2];
    if (pipe(p) == -1) { perror("pipe"); return EXIT_FAILURE; }
    pid_t a = fork();
    if (a == -1) { perror("fork"); return EXIT_FAILURE; }
    if (a == 0) {
        close(p[0]); close(STDOUT_FILENO);
        if (fcntl(p[1], F_DUPFD, STDOUT_FILENO) == -1) { perror("fcntl stdout"); _exit(1); }
        close(p[1]); execlp("ls", "ls", "-l", (char *)NULL); perror("execlp ls"); _exit(127);
    }
    pid_t b = fork();
    if (b == -1) { perror("fork"); return EXIT_FAILURE; }
    if (b == 0) {
        close(p[1]); close(STDIN_FILENO);
        if (fcntl(p[0], F_DUPFD, STDIN_FILENO) == -1) { perror("fcntl stdin"); _exit(1); }
        close(p[0]); execlp("wc", "wc", (char *)NULL); perror("execlp wc"); _exit(127);
    }
    close(p[0]); close(p[1]);
    int s1, s2; waitpid(a, &s1, 0); waitpid(b, &s2, 0);
    return (WIFEXITED(s1) && WEXITSTATUS(s1) == 0 && WIFEXITED(s2) && WEXITSTATUS(s2) == 0) ? 0 : 1;
}
