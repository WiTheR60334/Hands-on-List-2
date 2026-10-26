#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

key_t message_queue_key(void) { return ftok("/tmp", 'Q'); }
int main(void) {
    key_t key = message_queue_key();
    if (key == (key_t)-1) { perror("ftok"); return 1; }
    int id = msgget(key, IPC_CREAT | 0600);
    if (id == -1) { perror("msgget"); return 1; }
    printf("Message queue key: %ld\nMessage queue ID: %d\n", (long)key, id);
    puts("The same ftok(\"/tmp\", 'Q') key is used by Q42-Q46.");
    return 0;
}
