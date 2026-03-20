#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if(argc < 3){
        fprintf(2, "Usage: trace mask [process] [args...]\n");
        exit(1);
    }
    uint64 mask = atol(argv[1]);
    trace(mask);
    // exec swap memory to new process, but not metadata, so trace would still apply
    return exec(argv[2], argv+2);
}