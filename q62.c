#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void) {
    struct sigaction sa={0}; sa.sa_handler=SIG_IGN; sigemptyset(&sa.sa_mask);
    if(sigaction(SIGINT,&sa,NULL)==-1){perror("sigaction ignore");return 1;}
    puts("SIGINT ignored for 5 seconds; press Ctrl+C to test."); fflush(stdout); sleep(5);
    sa.sa_handler=SIG_DFL;
    if(sigaction(SIGINT,&sa,NULL)==-1){perror("sigaction default");return 1;}
    puts("SIGINT restored to default; raising it now terminates the process."); fflush(stdout); raise(SIGINT);
    return 0;
}
