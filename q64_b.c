#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char **argv) {
    if(argc!=2){fprintf(stderr,"Usage: %s PID\n",argv[0]);return 1;}
    char *end=NULL; long raw=strtol(argv[1],&end,10); if(!end||*end!='\0'||raw<=1){fprintf(stderr,"Invalid PID.\n");return 1;}
    pid_t pid=(pid_t)raw;
    if(kill(pid,SIGSTOP)==-1){perror("kill SIGSTOP");return 1;}
    printf("Sent SIGSTOP to PID %ld. That process is stopped, not handling the signal.\n",raw); fflush(stdout);
    sleep(3);
    if(kill(pid,SIGCONT)==-1){perror("kill SIGCONT");return 1;}
    puts("Sent SIGCONT so the first program resumes. SIGSTOP cannot be caught, blocked, or ignored.");
    return 0;
}
