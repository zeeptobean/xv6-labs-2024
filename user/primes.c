#include "kernel/types.h"
#include "user/user.h"

void __attribute__((noreturn)) prime(int fd_in) {
    int p = 0, n;
    if(read(fd_in, &p, 4) != 4) {
        close(fd_in);
        exit(0);
    }
    printf("prime %d\n", p);

    int fd[2];
    if(pipe(fd) < 0) {
        close(fd_in);
        printf("pipe error\n");
        exit(1);
    }
    if(fork() == 0) {
        close(fd_in);
        close(fd[1]);
        prime(fd[0]);
        exit(0);
    } else {
        close(fd[0]);
        while(read(fd_in, &n, 4) == 4) {
            if(n % p != 0) {
                write(fd[1], &n, 4);
            }
        }
        close(fd_in);
        close(fd[1]);
        wait(0);
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    int fd[2];
    if(pipe(fd) < 0) {
        printf("pipe error\n");
        exit(1);
    }
    if(fork() == 0) {
        close(fd[1]);
        prime(fd[0]);
        exit(0);
    } else {
        close(fd[0]);
        for(int i=2; i <= 280; i++) { 
            write(fd[1], &i, 4);
        }
        close(fd[1]);
        wait(0);
        exit(0);
    }
}

