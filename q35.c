#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static void redirect_or_die(int oldfd, int newfd) {
    if (dup2(oldfd, newfd) == -1) { perror("dup2"); _exit(1); }
}

int main(void) {
    int a[2], b[2];
    if (pipe(a) == -1 || pipe(b) == -1) { perror("pipe"); return EXIT_FAILURE; }
    pid_t p1 = fork();
    if (p1 == -1) { perror("fork"); return EXIT_FAILURE; }
    if (p1 == 0) {
        redirect_or_die(a[1], STDOUT_FILENO);
        close(a[0]); close(a[1]); close(b[0]); close(b[1]);
        execlp("ls", "ls", "-l", (char *)NULL); perror("exec ls"); _exit(127);
    }
    pid_t p2 = fork();
    if (p2 == -1) { perror("fork"); return EXIT_FAILURE; }
    if (p2 == 0) {
        redirect_or_die(a[0], STDIN_FILENO); redirect_or_die(b[1], STDOUT_FILENO);
        close(a[0]); close(a[1]); close(b[0]); close(b[1]);
        execlp("grep", "grep", "^d", (char *)NULL); perror("exec grep"); _exit(127);
    }
    pid_t p3 = fork();
    if (p3 == -1) { perror("fork"); return EXIT_FAILURE; }
    if (p3 == 0) {
        redirect_or_die(b[0], STDIN_FILENO);
        close(a[0]); close(a[1]); close(b[0]); close(b[1]);
        execlp("wc", "wc", (char *)NULL); perror("exec wc"); _exit(127);
    }
    close(a[0]); close(a[1]); close(b[0]); close(b[1]);
    int s; waitpid(p1, &s, 0); waitpid(p2, &s, 0); waitpid(p3, &s, 0);
    return 0;
}
