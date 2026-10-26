#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
static void handler(int sig) { (void)sig; const char msg[]="Caught SIGFPE with sigaction().\n"; (void)write(STDOUT_FILENO,msg,sizeof(msg)-1); _exit(0); }
int main(void) { struct sigaction sa={0}; sa.sa_handler=handler; sigemptyset(&sa.sa_mask); if(sigaction(SIGFPE,&sa,NULL)==-1){perror("sigaction");return 1;} puts("Raising SIGFPE explicitly."); fflush(stdout); raise(SIGFPE); return 0; }
