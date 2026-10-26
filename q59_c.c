#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
static void handler(int sig) { (void)sig; const char msg[] = "Caught SIGFPE using signal().\n"; (void)write(STDOUT_FILENO, msg, sizeof(msg)-1); _exit(0); }
int main(void) { if (signal(SIGFPE, handler) == SIG_ERR) { perror("signal"); return 1; } puts("Raising SIGFPE explicitly (avoids compiler-dependent division-by-zero behavior)."); fflush(stdout); raise(SIGFPE); return 0; }
