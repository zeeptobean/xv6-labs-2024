#include "types.h"

struct sysinfo {
    int freemem;
    int nproc;
    int nopenfiles;
};

int countfreemem(void);
int countfreeproc(void);
int countopenfiles(void);