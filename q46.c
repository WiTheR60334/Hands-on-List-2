#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(void) {
    key_t key = ftok("/tmp", 'Q'); if (key == (key_t)-1) { perror("ftok"); return 1; }
    int id = msgget(key, 0600); if (id == -1) { perror("msgget"); return 1; }
    if (msgctl(id, IPC_RMID, NULL) == -1) { perror("msgctl IPC_RMID"); return 1; }
    printf("Removed message queue ID %d.\n", id);
    return 0;
}
