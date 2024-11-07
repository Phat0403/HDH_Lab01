#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    int tracenum;

    if (argc < 2) {
        fprintf(2, "Usage: trace <trace_number> <command> [args...]\n");
        return -1;
    }

    tracenum = atoi(argv[1]);

    if (trace(tracenum) < 0) {
        fprintf(2, "Trace failed\n");
        return -1;
    }

    if (exec(argv[2], &argv[2]) < 0) {
        fprintf(2, "Exec failed\n");
        return -1;
    }

    return 0;
}
