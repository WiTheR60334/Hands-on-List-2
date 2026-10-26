#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>

static void show_time(const char *label, time_t t) {
    if (t == 0) { printf("%s: never/not recorded\n", label); return; }
    char b[64]; struct tm tm;
    if (localtime_r(&t, &tm) && strftime(b, sizeof(b), "%Y-%m-%d %H:%M:%S", &tm)) printf("%s: %s\n", label, b);
    else printf("%s: %lld\n", label, (long long)t);
}
int main(void) {
    key_t key = ftok("/tmp", 'Q'); if (key == (key_t)-1) { perror("ftok"); return 1; }
    int id = msgget(key, 0600); if (id == -1) { perror("msgget (run Q41 first)"); return 1; }
    struct msqid_ds ds;
    if (msgctl(id, IPC_STAT, &ds) == -1) { perror("msgctl IPC_STAT"); return 1; }
    printf("Key=%ld, queue ID=%d\n", (long)key, id);
    printf("Permission bits: %03o\n", ds.msg_perm.mode & 0777);
    printf("Owner UID/GID: %u/%u\n", (unsigned)ds.msg_perm.uid, (unsigned)ds.msg_perm.gid);
    printf("Creator UID/GID: %u/%u\n", (unsigned)ds.msg_perm.cuid, (unsigned)ds.msg_perm.cgid);
    show_time("Last message sent", ds.msg_stime);
    show_time("Last message received", ds.msg_rtime);
    show_time("Last queue change", ds.msg_ctime);
#ifdef __linux__
    printf("Bytes currently queued: %lu\n", (unsigned long)ds.__msg_cbytes);
#else
    printf("Bytes currently queued: %lu\n", (unsigned long)ds.msg_cbytes);
#endif
    printf("Number of messages: %lu\n", (unsigned long)ds.msg_qnum);
    printf("Maximum queue bytes: %lu\n", (unsigned long)ds.msg_qbytes);
    printf("PID of last msgsnd: %ld\n", (long)ds.msg_lspid);
    printf("PID of last msgrcv: %ld\n", (long)ds.msg_lrpid);
    return 0;
}
