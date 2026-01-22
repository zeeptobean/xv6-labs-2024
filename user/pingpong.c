#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int fd[2], fd2[2];
    const char data = 'a';
    pipe(fd);
    pipe(fd2);

    int child = fork();
    if(child < 0) {
        printf("fork failed\n");
        exit(1);
    } else if(child == 0) {
        //child: 
        char child_data = 0;
        close(fd[1]);
        read(fd[0], &child_data, 1);
        if(child_data == data) {
            int child_pid = getpid();
            printf("%d: received ping\n", child_pid);
        } else {
            printf("failed\n");
        }

        close(fd2[0]);
        write(fd2[1], &data, 1);
    } else {
        //parent
        close(fd[0]);
        write(fd[1], &data, 1);

        close(fd2[1]);
        char parent_data = 0;
        read(fd2[0], &parent_data, 1);
        if(parent_data == data) {
            int parent_pid = getpid();
            printf("%d: received pong\n", parent_pid);
        } else {
            printf("failed\n");
        }
    }
}