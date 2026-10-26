#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

static volatile sig_atomic_t fired = 0;
static void handler(int sig) { (void)sig; fired = 1; const char msg[] = "ITIMER_REAL expired (SIGALRM)\n"; (void)write(STDOUT_FILENO, msg, sizeof(msg) - 1); }
int main(void) {
    struct sigaction sa = {0}; sa.sa_handler = handler; sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) == -1) { perror("sigaction"); return 1; }
    struct itimerval it = { .it_value = { .tv_sec = 10, .tv_usec = 10 }, .it_interval = { .tv_sec = 10, .tv_usec = 10 } };
    printf("Wall-clock timer set for 10 seconds + 10 microseconds; waiting...\n"); fflush(stdout);
    if (setitimer(ITIMER_REAL, &it, NULL) == -1) { perror("setitimer"); return 1; }
    while (!fired) sleep(1);
    return 0;
}
