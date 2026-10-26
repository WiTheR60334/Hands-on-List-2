#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct message { long type; char text[256]; };
int main(int argc, char **argv) {
    int flags = 0;
    if (argc > 2 || (argc == 2 && strcmp(argv[1], "nowait") != 0 && strcmp(argv[1], "block") != 0)) {
        fprintf(stderr, "Usage: %s [block|nowait]\n", argv[0]); return 1;
    }
    if (argc == 2 && strcmp(argv[1], "nowait") == 0) flags = IPC_NOWAIT;
    key_t key = ftok("/tmp", 'Q'); if (key == (key_t)-1) { perror("ftok"); return 1; }
    int id = msgget(key, 0600); if (id == -1) { perror("msgget (run Q41 first)"); return 1; }
    struct message m;
    ssize_t n = msgrcv(id, &m, sizeof(m.text), 0, flags);
    if (n == -1) {
        if (errno == ENOMSG) { puts("IPC_NOWAIT: no message is available; msgrcv returned immediately."); return 0; }
        perror("msgrcv"); return 1;
    }
    printf("Received type %ld (%zd bytes): %s\n", m.type, n, m.text);
    return 0;
}
