#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct message { long type; char text[256]; };
int main(int argc, char **argv) {
    const char *text = argc > 1 ? argv[1] : "Hello from msgsnd";
    if (strlen(text) >= sizeof(((struct message *)0)->text)) { fprintf(stderr, "Message is too long (maximum 255 characters).\n"); return 1; }
    key_t key = ftok("/tmp", 'Q'); if (key == (key_t)-1) { perror("ftok"); return 1; }
    int id = msgget(key, IPC_CREAT | 0600); if (id == -1) { perror("msgget"); return 1; }
    struct message m = { .type = 1 };
    strcpy(m.text, text);
    if (msgsnd(id, &m, strlen(m.text) + 1, 0) == -1) { perror("msgsnd"); return 1; }
    printf("Sent message to queue ID %d: %s\n", id, m.text);
    puts("Verify with: ipcs -q");
    return 0;
}
