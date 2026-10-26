#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void show(const char *label, int name) {
    errno = 0;
    long value = sysconf(name);
    if (value == -1 && errno != 0) printf("%-46s unavailable (%s)\n", label, strerror(errno));
    else if (value == -1) printf("%-46s indeterminate\n", label);
    else printf("%-46s %ld\n", label, value);
}
int main(void) {
    show("Maximum combined argv+environment bytes for exec*", _SC_ARG_MAX);
    show("Maximum simultaneous processes per real user ID", _SC_CHILD_MAX);
    show("Clock ticks per second (_SC_CLK_TCK)", _SC_CLK_TCK);
    show("Maximum open file descriptors for this process", _SC_OPEN_MAX);
    show("Memory page size in bytes", _SC_PAGESIZE);
    show("Total physical memory pages", _SC_PHYS_PAGES);
    show("Currently available physical memory pages", _SC_AVPHYS_PAGES);
    return 0;
}
