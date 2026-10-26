#define _GNU_SOURCE
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <x86intrin.h>

int main(void) {
    volatile pid_t result = 0;
    unsigned int aux;
    _mm_lfence();
    uint64_t start = __rdtscp(&aux);
    for (int i = 0; i < 100; i++) result = getppid();
    _mm_lfence();
    uint64_t end = __rdtscp(&aux);
    printf("100 getppid() calls returned (last value): %ld\n", (long)result);
    printf("Timestamp-counter delta: %" PRIu64 " ticks\n", end - start);
    printf("Average: %.2f TSC ticks per call\n", (double)(end - start) / 100.0);
    puts("TSC ticks are not guaranteed to equal actual CPU core cycles; this program is x86-specific.");
    return 0;
}
