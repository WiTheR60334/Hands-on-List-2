#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

union semun { int val; struct semid_ds *buf; unsigned short *array; };
static int sem_change(int id, short delta) { struct sembuf op = { .sem_num = 0, .sem_op = delta, .sem_flg = SEM_UNDO }; return semop(id, &op, 1); }
int main(void) {
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600); if (shmid == -1) { perror("shmget"); return 1; }
    int *counter = shmat(shmid, NULL, 0); if (counter == (void *)-1) { perror("shmat"); shmctl(shmid, IPC_RMID, NULL); return 1; }
    *counter = 0;
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600); if (semid == -1) { perror("semget"); shmdt(counter); shmctl(shmid, IPC_RMID, NULL); return 1; }
    union semun arg; arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) { perror("semctl"); return 1; }
    for (int i = 0; i < 4; i++) {
        pid_t pid = fork();
        if (pid == -1) { perror("fork"); break; }
        if (pid == 0) {
            for (int j = 0; j < 5; j++) {
                if (sem_change(semid, -1) == -1) { perror("semop lock"); _exit(1); }
                int ticket = ++(*counter);
                printf("PID %ld got ticket %d\n", (long)getpid(), ticket); fflush(stdout);
                if (sem_change(semid, 1) == -1) { perror("semop unlock"); _exit(1); }
            }
            shmdt(counter); _exit(0);
        }
    }
    while (wait(NULL) > 0) {}
    printf("Total tickets issued: %d\n", *counter);
    shmdt(counter); shmctl(shmid, IPC_RMID, NULL); semctl(semid, 0, IPC_RMID);
    return 0;
}
