#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
static void handler(int sig) { (void)sig; const char msg[] = "Caught SIGALRM delivered by alarm().\n"; (void)write(STDOUT_FILENO, msg, sizeof(msg)-1); _exit(0); }
int main(void) { if (signal(SIGALRM, handler) == SIG_ERR) { perror("signal"); return 1; } puts("alarm(2): waiting about 2 seconds..."); fflush(stdout); alarm(2); for (;;) pause(); }
