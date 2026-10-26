#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int make_fifo(const char *path, int use_mknod) {
    int rc = use_mknod ? mknod(path, S_IFIFO | 0600, 0) : mkfifo(path, 0600);
    if (rc == -1 && errno == EEXIST) {
        printf("%s already exists; leaving it in place.\n", path);
        return 0;
    }
    return rc;
}

int main(int argc, char **argv) {
    if (argc != 3 || (strcmp(argv[1], "mknod") && strcmp(argv[1], "mkfifo"))) {
        fprintf(stderr, "Usage: %s mknod|mkfifo PATH\n", argv[0]);
        fprintf(stderr, "This demonstrates the C mknod() system interface or mkfifo() library function.\n");
        return EXIT_FAILURE;
    }
    if (make_fifo(argv[2], strcmp(argv[1], "mknod") == 0) == -1) {
        perror(argv[1]); return EXIT_FAILURE;
    }
    struct stat st;
    if (lstat(argv[2], &st) == -1) { perror("lstat"); return EXIT_FAILURE; }
    if (!S_ISFIFO(st.st_mode)) { fprintf(stderr, "%s exists but is not a FIFO.\n", argv[2]); return EXIT_FAILURE; }
    printf("FIFO ready: %s (mode %03o)\n", argv[2], st.st_mode & 0777);
    printf("Remove it with: rm -- '%s'\n", argv[2]);
    return 0;
}
