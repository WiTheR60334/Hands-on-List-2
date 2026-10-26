#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct args { int number; };
static void *worker(void *ptr) {
    struct args *a = ptr;
    printf("Thread %d: pthread_self() = %lu\n", a->number, (unsigned long)pthread_self());
    return NULL;
}
int main(void) {
    pthread_t threads[3]; struct args a[3];
    for (int i = 0; i < 3; i++) {
        a[i].number = i + 1;
        int rc = pthread_create(&threads[i], NULL, worker, &a[i]);
        if (rc != 0) { fprintf(stderr, "pthread_create failed: %d\n", rc); return 1; }
    }
    for (int i = 0; i < 3; i++) {
        int rc = pthread_join(threads[i], NULL);
        if (rc != 0) { fprintf(stderr, "pthread_join failed: %d\n", rc); return 1; }
    }
    puts("All three threads have finished.");
    return 0;
}
