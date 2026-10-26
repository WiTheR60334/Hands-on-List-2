#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void) {
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) { perror("signal ignore"); return 1; }
    puts("SIGINT is ignored for 5 seconds. Press Ctrl+C now to test ignoring."); fflush(stdout);
    sleep(5);
    if (signal(SIGINT, SIG_DFL) == SIG_ERR) { perror("signal default"); return 1; }
    puts("SIGINT reset to default; raising SIGINT now terminates this process."); fflush(stdout);
    raise(SIGINT);
    return 0;
}
