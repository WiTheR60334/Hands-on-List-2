#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    pid_t parent = getpid();
    printf("Original parent PID: %ld\n", (long)parent); fflush(stdout);
    pid_t child = fork();
    if (child == -1) { perror("fork"); return 1; }
    if (child == 0) {
        printf("Child PID %ld will send SIGKILL to parent PID %ld.\n", (long)getpid(), (long)parent); fflush(stdout);
        sleep(2);
        if (kill(parent, SIGKILL) == -1) { perror("child kill parent"); _exit(1); }
        sleep(1); /* Give the kernel/init process time to reparent this orphan. */
        printf("Child is now an orphan/adopted process; new PPID=%ld\n", (long)getppid()); fflush(stdout);
        _exit(0);
    }
    puts("Parent waiting; it cannot catch or handle SIGKILL."); fflush(stdout);
    for (;;) pause();
}
