#define _GNU_SOURCE
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static void *handle_client(void *arg) {
    int fd = *(int *)arg; free(arg);
    char buf[512]; size_t n = 0;
    while (n + 1 < sizeof(buf)) { char c; ssize_t r = recv(fd, &c, 1, 0); if (r == 0) break; if (r == -1) { if (errno == EINTR) continue; close(fd); return NULL; } buf[n++] = c; if (c == '\n') break; }
    buf[n] = '\0'; dprintf(STDOUT_FILENO, "Thread 0x%lx handled: %s", (unsigned long)pthread_self(), buf);
    const char reply[] = "Handled by pthread-based server\n"; (void)send(fd, reply, sizeof(reply) - 1, 0);
    close(fd); return NULL;
}
int main(int argc, char **argv) {
    int port = argc > 1 ? atoi(argv[1]) : 5002;
    if (port < 1 || port > 65535) return 1;
    int s = socket(AF_INET, SOCK_STREAM, 0); if (s == -1) { perror("socket"); return 1; }
    int yes=1; setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    struct sockaddr_in a = { .sin_family=AF_INET, .sin_port=htons((unsigned short)port), .sin_addr.s_addr=htonl(INADDR_ANY) };
    if (bind(s, (struct sockaddr *)&a, sizeof(a)) == -1 || listen(s, 16) == -1) { perror("bind/listen"); close(s); return 1; }
    printf("Thread server listening on port %d.\n", port); fflush(stdout);
    for (;;) {
        int c = accept(s, NULL, NULL); if (c == -1) { if (errno == EINTR) continue; perror("accept"); break; }
        int *arg = malloc(sizeof(*arg)); if (!arg) { perror("malloc"); close(c); continue; } *arg=c;
        pthread_t thread; int rc = pthread_create(&thread, NULL, handle_client, arg);
        if (rc != 0) { errno=rc; perror("pthread_create"); free(arg); close(c); continue; }
        rc = pthread_detach(thread); if (rc != 0) { errno=rc; perror("pthread_detach"); }
    }
    close(s); return 1;
}
