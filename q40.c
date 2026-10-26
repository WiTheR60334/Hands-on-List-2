#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

int main(void) {
    struct rlimit lim;
    if (getrlimit(RLIMIT_NOFILE, &lim) == -1) { perror("getrlimit RLIMIT_NOFILE"); return 1; }
    printf("Open-file descriptor limit: soft=");
    if (lim.rlim_cur == RLIM_INFINITY) printf("infinity"); else printf("%llu", (unsigned long long)lim.rlim_cur);
    printf(", hard=");
    if (lim.rlim_max == RLIM_INFINITY) printf("infinity\n"); else printf("%llu\n", (unsigned long long)lim.rlim_max);

    int p[2]; if (pipe(p) == -1) { perror("pipe"); return 1; }
#ifdef F_GETPIPE_SZ
    int size = fcntl(p[0], F_GETPIPE_SZ);
    if (size == -1) perror("fcntl F_GETPIPE_SZ"); else printf("Pipe buffer capacity: %d bytes\n", size);
#else
    puts("F_GETPIPE_SZ is not available in these headers/platform.");
#endif
    close(p[0]); close(p[1]);
    puts("Note: RLIMIT_NOFILE limits file descriptors, not only regular files.");
    return 0;
}
