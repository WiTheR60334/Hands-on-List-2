#define _GNU_SOURCE
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <unistd.h>
static void handler(int sig) { const char msg[] = "Caught SIGSEGV using sigaction(); exiting.\n"; (void)sig; (void)write(STDOUT_FILENO, msg, sizeof(msg)-1); _exit(0); }
int main(void) { struct sigaction sa={0}; sa.sa_handler=handler; sigemptyset(&sa.sa_mask); if (sigaction(SIGSEGV,&sa,NULL)==-1) { perror("sigaction"); return 1; } puts("Triggering an intentional invalid memory write..."); fflush(stdout); long page_size=sysconf(_SC_PAGESIZE); void *p=mmap(NULL,(size_t)page_size,PROT_NONE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0); if(p==MAP_FAILED){perror("mmap");return 1;} *(volatile int *)p=9; return 1; }
