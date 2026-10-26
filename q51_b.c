#define _GNU_SOURCE
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int send_all(int fd, const char *buf, size_t len) {
    while (len) { ssize_t n = send(fd, buf, len, 0); if (n == -1) { if (errno == EINTR) continue; return -1; } if (n == 0) return -1; buf += n; len -= (size_t)n; }
    return 0;
}
static ssize_t recv_line(int fd, char *buf, size_t cap) {
    size_t n = 0;
    while (n + 1 < cap) { char c; ssize_t r = recv(fd, &c, 1, 0); if (r == 0) break; if (r == -1) { if (errno == EINTR) continue; return -1; } buf[n++] = c; if (c == '\n') break; }
    buf[n] = '\0'; return (ssize_t)n;
}
int main(int argc, char **argv) {
    int port = argc > 1 ? atoi(argv[1]) : 5000;
    if (port < 1 || port > 65535) { fprintf(stderr, "Invalid port.\n"); return 1; }
    int server = socket(AF_INET, SOCK_STREAM, 0); if (server == -1) { perror("socket"); return 1; }
    int yes = 1; if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) perror("setsockopt");
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons((unsigned short)port), .sin_addr.s_addr = htonl(INADDR_ANY) };
    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) == -1) { perror("bind"); close(server); return 1; }
    if (listen(server, 5) == -1) { perror("listen"); close(server); return 1; }
    printf("TCP server listening on port %d; connect from another machine using this machine's IP.\n", port); fflush(stdout);
    int client = accept(server, NULL, NULL); if (client == -1) { perror("accept"); close(server); return 1; }
    char msg[512]; ssize_t n = recv_line(client, msg, sizeof(msg));
    if (n == -1) perror("recv"); else printf("Client sent: %s", msg);
    const char reply[] = "Server received your message.\n";
    if (send_all(client, reply, sizeof(reply) - 1) == -1) perror("send");
    close(client); close(server);
    return n == -1 ? 1 : 0;
}
