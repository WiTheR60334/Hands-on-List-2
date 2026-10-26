#define _GNU_SOURCE
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

static void reap_children(int sig) { (void)sig; while (waitpid(-1, NULL, WNOHANG) > 0) {} }
static void handle_client(int fd) {
    char buf[512]; size_t n = 0;
    while (n + 1 < sizeof(buf)) { char c; ssize_t r = recv(fd, &c, 1, 0); if (r == 0) break; if (r == -1) { if (errno == EINTR) continue; return; } buf[n++] = c; if (c == '\n') break; }
    buf[n] = '\0'; dprintf(STDOUT_FILENO, "Child PID %ld handled: %s", (long)getpid(), buf);
    const char reply[] = "Handled by fork-based server\n"; (void)send(fd, reply, sizeof(reply) - 1, 0);
}
int main(int argc, char **argv) {
    int port = argc > 1 ? atoi(argv[1]) : 5001;
    if (port < 1 || port > 65535) return 1;
    struct sigaction sa = {0}; sa.sa_handler = reap_children; sigemptyset(&sa.sa_mask); sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) { perror("sigaction SIGCHLD"); return 1; }
    int s = socket(AF_INET, SOCK_STREAM, 0); if (s == -1) { perror("socket"); return 1; }
    int yes = 1; setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    struct sockaddr_in a = { .sin_family=AF_INET, .sin_port=htons((unsigned short)port), .sin_addr.s_addr=htonl(INADDR_ANY) };
    if (bind(s, (struct sockaddr *)&a, sizeof(a)) == -1 || listen(s, 16) == -1) { perror("bind/listen"); close(s); return 1; }
    printf("Fork server listening on port %d.\n", port); fflush(stdout);
    for (;;) {
        int c = accept(s, NULL, NULL); if (c == -1) { if (errno == EINTR) continue; perror("accept"); break; }
        pid_t pid = fork();
        if (pid == -1) { perror("fork"); close(c); continue; }
        if (pid == 0) { close(s); handle_client(c); close(c); _exit(0); }
        close(c); /* Parent returns to accept; child owns its connected descriptor. */
    }
    close(s); return 1;
}
