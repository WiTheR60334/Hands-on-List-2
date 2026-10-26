#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
static void handler(int sig) { (void)sig; const char msg[] = "Caught SIGPROF from ITIMER_PROF.\n"; (void)write(STDOUT_FILENO, msg, sizeof(msg)-1); _exit(0); }
int main(void) { if (signal(SIGPROF, handler) == SIG_ERR) { perror("signal"); return 1; } struct itimerval t = { .it_value={.tv_sec=2}, .it_interval={0,0} }; puts("ITIMER_PROF set for 2 seconds of user+system CPU time; busy-waiting..."); fflush(stdout); if (setitimer(ITIMER_PROF, &t, NULL) == -1) { perror("setitimer"); return 1; } volatile unsigned long x=0; while (1) x++; }
