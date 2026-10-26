#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(void) {
    key_t key = ftok("/tmp", 'Q'); if (key == (key_t)-1) { perror("ftok"); return 1; }
    int id = msgget(key, 0600); if (id == -1) { perror("msgget (run Q41 first)"); return 1; }
    struct msqid_ds ds;
    if (msgctl(id, IPC_STAT, &ds) == -1) { perror("msgctl IPC_STAT"); return 1; }
    printf("Old permissions: %03o\n", ds.msg_perm.mode & 0777);
    ds.msg_perm.mode = (ds.msg_perm.mode & ~0777) | 0600;
    if (msgctl(id, IPC_SET, &ds) == -1) { perror("msgctl IPC_SET (must own queue or be privileged)"); return 1; }
    if (msgctl(id, IPC_STAT, &ds) == -1) { perror("msgctl verify"); return 1; }
    printf("New permissions: %03o\n", ds.msg_perm.mode & 0777);
    return 0;
}
