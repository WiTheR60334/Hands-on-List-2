#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun { int val; struct semid_ds *buf; unsigned short *array; };
static int create_set(char project, int initial_value) {
    key_t key = ftok("/tmp", project);
    if (key == (key_t)-1) { perror("ftok"); return -1; }
    int id = semget(key, 1, IPC_CREAT | 0600);
    if (id == -1) { perror("semget"); return -1; }
    union semun arg; arg.val = initial_value;
    if (semctl(id, 0, SETVAL, arg) == -1) { perror("semctl SETVAL"); return -1; }
    printf("%s semaphore: key=%ld id=%d initial value=%d\n", project == 'B' ? "Binary" : "Counting", (long)key, id, initial_value);
    return id;
}
int main(void) {
    int binary_id = create_set('B', 1); if (binary_id == -1) return 1;
    int counting_id = create_set('C', 3); if (counting_id == -1) return 1;
    puts("Inspect with: ipcs -s. Remove with: ipcrm -s SEMID (after noting the IDs above).");
    return 0;
}
