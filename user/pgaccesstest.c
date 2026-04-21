#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

char *testname = "???";

void
err(char *why) {
  printf("pgaccesstest failed: %s\n", why);
  exit(1);
}

int
main(int argc, char *argv[]) {
    char *buf;
    uint64 abits;
    printf("pgaccesstest starting...\n");

    printf("Test 1: Checking access bits of 64-pages buffer...\n");
    buf = malloc(64 * PGSIZE);
    if(pgaccess(buf, 64, &abits) < 0) {
        err("first run to reset access bit");
    }
    buf[PGSIZE * 1] += 1;
    buf[PGSIZE * 2] += 1;
    buf[PGSIZE * 30] += 1;
    buf[PGSIZE * 51] += 1;
    buf[PGSIZE * 63] += 1;
    if(pgaccess(buf, 64, &abits) < 0) {
        err("second run to check access bit");
    }
    if(abits != ((1ULL << 1) | (1ULL << 2) | (1ULL << 30) | (1ULL << 51) | (1ULL << 63))) {
        err("incorrect access bits set");
    }
    free(buf);
    printf("OK. Bitmask: %p\n", (void*)abits);

    printf("Test 2: Trying pgaccess on >64 pages...\n");
    buf = malloc(65 * PGSIZE);
    if(pgaccess(buf, 65, &abits) < 0) {
        printf("OK.\n");
    } else {
        err("pgaccess should fail when pg_cnt > 64");
    }
    free(buf);

    printf("Test 3: Trying pgaccess on unmapped pages...\n");
    if(pgaccess((void*)0x12345678, 1, &abits) < 0) {
        printf("OK.\n");
    } else {
        err("pgaccess should fail on unmapped pages");
    }

    exit(0);
}


