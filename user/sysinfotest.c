#include "kernel/types.h"
#include "kernel/sysinfo.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define UNREFERENCE_PARAMETER(x) (void)(x)

int call_sysinfo() {
    struct sysinfo info;
    if (sysinfo(&info) < 0) {
        printf("sysinfo failed\n");
        return 0;
    } else {
        printf("free memory: %d bytes\n", info.freemem);
        printf("free processes: %d\n", info.nproc);
        printf("open files: %d\n", info.nopenfiles);
        return 1;
    }
}

int openfile(const char *filename) {
    int fd = open(filename, O_CREATE | O_WRONLY | O_TRUNC);
    if (fd < 0) {
        printf("Failed to open file: %s\n", filename);
        return -1;
    }
    return fd;
}

const char msg[] = "Hello, xv6!";

int main(void) {
    printf("Original sysinfo:\n");
    if (!call_sysinfo()) return 1;

    int fd = openfile("testfile.txt");
    if (fd < 0) return 1;
    write(fd, msg, strlen(msg));
    int fd2 = openfile("testfile2.txt");
    if (fd2 < 0) return 1;
    printf("After opening files:\n");
    if (!call_sysinfo()) return 1;
    
    close(fd2);
    printf("After closing fd2 files:\n");
    if (!call_sysinfo()) return 1;

    close(fd);
    printf("After closing both files:\n");
    if (!call_sysinfo()) return 1;

    (void) sbrk(4096);

    printf("After allocating a page (4096 bytes) using sbrk:\n");
    if (!call_sysinfo()) return 1;

    (void) sbrk(-4096);
    printf("After de-allocating a page using sbrk:\n");
    if (!call_sysinfo()) return 1;

    printf("After free:\n");
    if (!call_sysinfo()) return 1;

    if(fork() == 0) {
        printf("In child process:\n");
        if (!call_sysinfo()) return 1;
        exit(0);
    } else {
        wait(0);
        printf("After child process exits:\n");
        if (!call_sysinfo()) return 1;
    }

    (void) sbrk(4 * 4096);
    fd = openfile("testfile3.txt");
    printf("Allocating 4 pages and open 1 file then fork\n");
    if(fork() == 0) {
        printf("In child process\n");
        if (!call_sysinfo()) return 1;
        exit(0);
    } else {
        wait(0);
        exit(0);
    }
}