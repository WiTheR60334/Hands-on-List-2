#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define A_TO_B "./q38_a_to_b"
#define B_TO_A "./q38_b_to_a"

static int ensure_fifo(const char *p) { if (mkfifo(p, 0600) == -1 && errno != EEXIST) { perror("mkfifo"); return -1; } return 0; }
static int write_line(int fd, const char *s) { size_t n = strlen(s); while (n) { ssize_t k = write(fd, s, n); if (k == -1) return -1; s += k; n -= (size_t)k; } return 0; }
static ssize_t read_line(int fd, char *buf, size_t cap) { size_t n = 0; while (n + 1 < cap) { char c; ssize_t k = read(fd, &c, 1); if (k == 0) break; if (k == -1) return -1; buf[n++] = c; if (c == '\n') break; } buf[n] = '\0'; return (ssize_t)n; }

int main(void) {
    if (ensure_fifo(A_TO_B) == -1 || ensure_fifo(B_TO_A) == -1) return 1;
    puts("Peer A: waiting for peer B..."); fflush(stdout);
    int out = open(A_TO_B, O_WRONLY); if (out == -1) { perror("open A_TO_B"); return 1; }
    int in = open(B_TO_A, O_RDONLY); if (in == -1) { perror("open B_TO_A"); close(out); return 1; }
    if (write_line(out, "Hello from peer A\n") == -1) perror("write");
    char buf[256]; ssize_t n = read_line(in, buf, sizeof(buf));
    if (n == -1) perror("read"); else printf("Peer A received: %s", buf);
    close(in); close(out); unlink(A_TO_B); unlink(B_TO_A);
    return n == -1 ? 1 : 0;
}
