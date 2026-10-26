#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
static void handler(int sig) { (void)sig; const char msg[] = "Caught SIGINT (Ctrl+C). Exiting.\n"; (void)write(STDOUT_FILENO, msg, sizeof(msg)-1); _exit(0); }
int main(void) { if (signal(SIGINT, handler) == SIG_ERR) { perror("signal"); return 1; } puts("Press Ctrl+C to deliver SIGINT."); fflush(stdout); for (;;) pause(); }
