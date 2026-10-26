#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
static void handler(int sig) { (void)sig; const char msg[]="This handler should never run for SIGSTOP.\n"; (void)write(STDOUT_FILENO,msg,sizeof(msg)-1); }
int main(void) {
    struct sigaction sa={0}; sa.sa_handler=handler; sigemptyset(&sa.sa_mask);
    if(sigaction(SIGSTOP,&sa,NULL)==-1) printf("Cannot install SIGSTOP handler: %s\n",strerror(errno));
    printf("Program 1 running, PID=%ld. Another terminal can run program2 with this PID.\n",(long)getpid()); fflush(stdout);
    for (;;) { puts("Program 1 is running..."); fflush(stdout); sleep(1); }
}
