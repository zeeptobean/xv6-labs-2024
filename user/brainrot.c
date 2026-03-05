#include "kernel/types.h"
#include "user/user.h"

int main(void) {
    printf("%d\n", sixseven());
    printf("random: %lu\n", random(0));
    exit(0);
}