#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

static void print_limit(const char *name, int resource) {
    struct rlimit r;
    if (getrlimit(resource, &r) == -1) { perror(name); return; }
    printf("%-18s soft=", name);
    if (r.rlim_cur == RLIM_INFINITY) printf("infinity"); else printf("%llu", (unsigned long long)r.rlim_cur);
    printf(" hard=");
    if (r.rlim_max == RLIM_INFINITY) printf("infinity\n"); else printf("%llu\n", (unsigned long long)r.rlim_max);
}
int main(void) {
    print_limit("RLIMIT_CPU", RLIMIT_CPU);
    print_limit("RLIMIT_FSIZE", RLIMIT_FSIZE);
    print_limit("RLIMIT_DATA", RLIMIT_DATA);
    print_limit("RLIMIT_STACK", RLIMIT_STACK);
    print_limit("RLIMIT_CORE", RLIMIT_CORE);
    print_limit("RLIMIT_NOFILE", RLIMIT_NOFILE);
#ifdef RLIMIT_NPROC
    print_limit("RLIMIT_NPROC", RLIMIT_NPROC);
#endif
#ifdef RLIMIT_AS
    print_limit("RLIMIT_AS", RLIMIT_AS);
#endif
#ifdef RLIMIT_MEMLOCK
    print_limit("RLIMIT_MEMLOCK", RLIMIT_MEMLOCK);
#endif
#ifdef RLIMIT_MSGQUEUE
    print_limit("RLIMIT_MSGQUEUE", RLIMIT_MSGQUEUE);
#endif
    return 0;
}
