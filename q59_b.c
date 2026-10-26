#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
static void handler(int sig) { (void)sig; const char msg[] = "Caught SIGALRM from ITIMER_REAL.\n"; (void)write(STDOUT_FILENO, msg, sizeof(msg)-1); _exit(0); }
int main(void) { if (signal(SIGALRM, handler) == SIG_ERR) { perror("signal"); return 1; } struct itimerval t = { .it_value={.tv_sec=2}, .it_interval={0,0} }; puts("ITIMER_REAL set for 2 seconds..."); fflush(stdout); if (setitimer(ITIMER_REAL, &t, NULL) == -1) { perror("setitimer"); return 1; } for (;;) pause(); }
