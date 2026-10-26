#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

union semun { int val; struct semid_ds *buf; unsigned short *array; };
static int change(int id, short delta) { struct sembuf op = {0, delta, SEM_UNDO}; return semop(id, &op, 1); }
int main(void) {
    int shmid = shmget(IPC_PRIVATE, 256, IPC_CREAT | 0600); if (shmid == -1) { perror("shmget"); return 1; }
    char *shared = shmat(shmid, NULL, 0); if (shared == (void *)-1) { perror("shmat"); shmctl(shmid, IPC_RMID, NULL); return 1; }
    strcpy(shared, "initial");
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600); if (semid == -1) { perror("semget"); return 1; }
    union semun arg; arg.val = 1; if (semctl(semid, 0, SETVAL, arg) == -1) { perror("SETVAL"); return 1; }
    for (int i = 0; i < 3; i++) {
        pid_t p = fork(); if (p == -1) { perror("fork"); break; }
        if (p == 0) {
            if (change(semid, -1) == -1) _exit(1);
            snprintf(shared, 256, "Written atomically by PID %ld", (long)getpid());
            printf("PID %ld wrote: %s\n", (long)getpid(), shared); fflush(stdout);
            sleep(1); /* Hold lock briefly so contention is visible. */
            change(semid, 1); shmdt(shared); _exit(0);
        }
    }
    while (wait(NULL) > 0) {}
    printf("Final shared-memory value: %s\n", shared);
    shmdt(shared); shmctl(shmid, IPC_RMID, NULL); semctl(semid, 0, IPC_RMID);
    return 0;
}
