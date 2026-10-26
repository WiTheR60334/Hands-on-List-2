#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

union semun { int val; struct semid_ds *buf; unsigned short *array; };
static int change(int id, unsigned short which, short delta) {
    struct sembuf op = { .sem_num = which, .sem_op = delta, .sem_flg = 0 };
    return semop(id, &op, 1);
}
static void worker(int id, unsigned short first, unsigned short second, int ready_fd, int gate_fd, const char *name) {
    if (change(id, first, -1) == -1) { perror("first lock"); _exit(1); }
    dprintf(STDOUT_FILENO, "%s acquired semaphore %u\n", name, first);
    if (write(ready_fd, "R", 1) != 1) _exit(1);
    char go;
    if (read(gate_fd, &go, 1) != 1) _exit(1);
    dprintf(STDOUT_FILENO, "%s now requests semaphore %u and will block: deadlock demonstration\n", name, second);
    if (change(id, second, -1) == -1) {
        dprintf(STDOUT_FILENO, "%s unblocked because the parent removed the semaphore set (%s).\n", name, strerror(errno));
        _exit(0);
    }
    _exit(0);
}
int main(void) {
    int semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0600); if (semid == -1) { perror("semget"); return 1; }
    union semun arg; unsigned short vals[2] = {1, 1}; arg.array = vals;
    if (semctl(semid, 0, SETALL, arg) == -1) { perror("SETALL"); semctl(semid, 0, IPC_RMID); return 1; }
    int ready[2], gate[2];
    if (pipe(ready) == -1 || pipe(gate) == -1) { perror("pipe"); semctl(semid, 0, IPC_RMID); return 1; }
    pid_t a = fork();
    if (a == -1) { perror("fork"); semctl(semid, 0, IPC_RMID); return 1; }
    if (a == 0) { close(ready[0]); close(gate[1]); worker(semid, 0, 1, ready[1], gate[0], "Process A"); }
    pid_t b = fork();
    if (b == -1) { perror("fork"); kill(a, SIGKILL); semctl(semid, 0, IPC_RMID); return 1; }
    if (b == 0) { close(ready[0]); close(gate[1]); worker(semid, 1, 0, ready[1], gate[0], "Process B"); }
    close(ready[1]); close(gate[0]);
    char tokens[2]; if (read(ready[0], &tokens[0], 1) != 1 || read(ready[0], &tokens[1], 1) != 1) { perror("read ready"); }
    write(gate[1], "GG", 2); close(gate[1]); close(ready[0]);
    puts("Both processes hold one semaphore and request the one held by the other. Waiting 2 seconds, then cleaning up safely..."); fflush(stdout);
    sleep(2);
    if (semctl(semid, 0, IPC_RMID) == -1) perror("semctl IPC_RMID");
    while (wait(NULL) > 0) {}
    puts("Semaphore set removed; blocked semop calls returned with an error, so the demo does not leave processes stuck.");
    return 0;
}
