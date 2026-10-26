#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

static void show(const char *label, const struct rlimit *r) {
    printf("%s soft=", label);
    if (r->rlim_cur == RLIM_INFINITY) printf("infinity"); else printf("%llu", (unsigned long long)r->rlim_cur);
    printf(" hard=");
    if (r->rlim_max == RLIM_INFINITY) printf("infinity\n"); else printf("%llu\n", (unsigned long long)r->rlim_max);
}
int main(void) {
    struct rlimit old, new_limit;
    if (getrlimit(RLIMIT_NOFILE, &old) == -1) { perror("getrlimit"); return 1; }
    show("Before:", &old);
    new_limit = old;
    /* Lower the soft limit to at most 64; do not try to raise it or change the hard limit. */
    if (new_limit.rlim_cur == RLIM_INFINITY || new_limit.rlim_cur > 64) new_limit.rlim_cur = 64;
    if (setrlimit(RLIMIT_NOFILE, &new_limit) == -1) { perror("setrlimit"); return 1; }
    if (getrlimit(RLIMIT_NOFILE, &new_limit) == -1) { perror("getrlimit verify"); return 1; }
    show("After:", &new_limit);
    puts("The new soft limit affects this process and its future children, not the whole system.");
    return 0;
}
