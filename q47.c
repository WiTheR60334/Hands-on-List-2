#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/resource.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    key_t key = ftok("/tmp", 'S'); if (key == (key_t)-1) { perror("ftok"); return 1; }
    int shmid = shmget(key, 1024, IPC_CREAT | 0600); if (shmid == -1) { perror("shmget"); return 1; }
    char *rw = shmat(shmid, NULL, 0); if (rw == (void *)-1) { perror("shmat writable"); shmctl(shmid, IPC_RMID, NULL); return 1; }
    strcpy(rw, "Data stored in System V shared memory");
    printf("Parent wrote: %s\n", rw);

    pid_t pid = fork();
    if (pid == -1) { perror("fork"); shmdt(rw); shmctl(shmid, IPC_RMID, NULL); return 1; }
    if (pid == 0) {
        struct rlimit core_limit = {0, 0}; (void)setrlimit(RLIMIT_CORE, &core_limit);
        char *ro = shmat(shmid, NULL, SHM_RDONLY);
        if (ro == (void *)-1) { perror("child shmat SHM_RDONLY"); _exit(2); }
        dprintf(STDOUT_FILENO, "Child read-only mapping sees: %s\n", ro);
        dprintf(STDOUT_FILENO, "Now attempting a write through the read-only mapping; SIGSEGV is expected.\n");
        volatile char *readonly_bytes = (volatile char *)ro;
        readonly_bytes[0] = 'X'; /* Deliberate fault: SHM_RDONLY maps the pages read-only. */
        (void)shmdt(ro);
        _exit(0);
    }
    int status;
    if (waitpid(pid, &status, 0) == -1) perror("waitpid");
    else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV) puts("Confirmed: writing through SHM_RDONLY caused SIGSEGV.");
    else printf("Read-only write test ended with status %d (unexpected on Linux).\n", status);

    printf("Parent's writable mapping still contains: %s\n", rw);
    if (shmdt(rw) == -1) perror("shmdt");
    if (shmctl(shmid, IPC_RMID, NULL) == -1) perror("shmctl IPC_RMID");
    else puts("Shared-memory segment marked for removal.");
    puts("System V uses SHM_RDONLY, not O_RDONLY, for read-only shmat().");
    return 0;
}
