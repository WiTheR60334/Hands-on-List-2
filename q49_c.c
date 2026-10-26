#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun { int val; struct semid_ds *buf; unsigned short *array; };
int main(void) {
    int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600); if (id == -1) { perror("semget"); return 1; }
    union semun arg; arg.val = 1;
    if (semctl(id, 0, SETVAL, arg) == -1) { perror("SETVAL"); semctl(id, 0, IPC_RMID); return 1; }
    printf("Created semaphore ID %d.\n", id);
    if (semctl(id, 0, IPC_RMID) == -1) { perror("semctl IPC_RMID"); return 1; }
    puts("Semaphore removed with semctl(IPC_RMID).");
    return 0;
}
