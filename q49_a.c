#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

static int change(int id, short delta) { struct sembuf op = {0, delta, SEM_UNDO}; return semop(id, &op, 1); }
int main(void) {
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600); if (semid == -1) { perror("semget"); return 1; }
    union semun_local { int val; struct semid_ds *buf; unsigned short *array; } arg; arg.val = 2;
    if (semctl(semid, 0, SETVAL, arg) == -1) { perror("SETVAL"); semctl(semid, 0, IPC_RMID); return 1; }
    puts("There are two identical pseudo-resources; at most two workers may hold a permit at once."); fflush(stdout);
    for (int i = 0; i < 5; i++) {
        pid_t p = fork(); if (p == -1) { perror("fork"); break; }
        if (p == 0) {
            if (change(semid, -1) == -1) { perror("wait semaphore"); _exit(1); }
            printf("PID %ld acquired a resource\n", (long)getpid()); fflush(stdout);
            sleep(2);
            printf("PID %ld released a resource\n", (long)getpid()); fflush(stdout);
            if (change(semid, 1) == -1) _exit(1);
            _exit(0);
        }
    }
    while (wait(NULL) > 0) {}
    if (semctl(semid, 0, IPC_RMID) == -1) perror("IPC_RMID");
    return 0;
}
