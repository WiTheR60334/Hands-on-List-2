#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

static volatile sig_atomic_t fired = 0;
static void handler(int sig) { (void)sig; fired = 1; const char msg[] = "ITIMER_VIRTUAL expired (SIGVTALRM)\n"; (void)write(STDOUT_FILENO, msg, sizeof(msg) - 1); }
int main(void) {
    struct sigaction sa = {0}; sa.sa_handler = handler; sigemptyset(&sa.sa_mask);
    if (sigaction(SIGVTALRM, &sa, NULL) == -1) { perror("sigaction"); return 1; }
    struct itimerval it = { .it_value = { .tv_sec = 10, .tv_usec = 10 }, .it_interval = { .tv_sec = 10, .tv_usec = 10 } };
    puts("CPU-user-time timer set for 10 seconds + 10 microseconds; busy-waiting for expiry..."); fflush(stdout);
    if (setitimer(ITIMER_VIRTUAL, &it, NULL) == -1) { perror("setitimer"); return 1; }
    while (!fired) __asm__ volatile("" ::: "memory");
    return 0;
}
