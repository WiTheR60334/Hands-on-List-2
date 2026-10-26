#define _GNU_SOURCE
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
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
    if (argc < 2 || argc > 4) { fprintf(stderr, "Usage: %s SERVER_IPV4 [PORT] [MESSAGE]\n", argv[0]); return 1; }
    int port = argc > 2 ? atoi(argv[2]) : 5000;
    if (port < 1 || port > 65535) { fprintf(stderr, "Invalid port.\n"); return 1; }
    int fd = socket(AF_INET, SOCK_STREAM, 0); if (fd == -1) { perror("socket"); return 1; }
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons((unsigned short)port) };
    if (inet_pton(AF_INET, argv[1], &addr.sin_addr) != 1) { fprintf(stderr, "Invalid IPv4 address.\n"); close(fd); return 1; }
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) { perror("connect"); close(fd); return 1; }
    const char *body = argc > 3 ? argv[3] : "Hello from client";
    char msg[512]; int len = snprintf(msg, sizeof(msg), "%s\n", body);
    if (len < 0 || (size_t)len >= sizeof(msg)) { fprintf(stderr, "Message too long.\n"); close(fd); return 1; }
    if (send_all(fd, msg, (size_t)len) == -1) { perror("send"); close(fd); return 1; }
    char reply[256]; ssize_t n = recv_line(fd, reply, sizeof(reply));
    if (n == -1) perror("recv"); else printf("Server replied: %s", reply);
    close(fd); return n == -1 ? 1 : 0;
}
