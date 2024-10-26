#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    char buf[512];
    char *args[MAXARG];
    int i, j, n;

    if (argc < 2) {
        fprintf(2, "usage: xargs [-n count] command ...\n");
        exit(1);
    }

    int n_flag = 0;  
    int count = 0;   // Num of args
    int base_args = argc - 1;

    // Check -n
    if (strcmp(argv[1], "-n") == 0) {
        n_flag = 1;
        count = atoi(argv[2]);
        base_args = argc - 3; // Update base args
    }

    // copy base command to xargs
    for (i = 0; i < base_args; i++) {
        args[i] = argv[n_flag ? i + 3 : i + 1];
    }

    while ((n = read(0, buf, sizeof(buf))) > 0) {
        int len = 0;
        int num_args = base_args; 
        for (j = 0; j < n; j++) {
            if (buf[j] == ' ' || buf[j] == '\n') {
                buf[j] = 0; // End of str
                args[num_args++] = &buf[len]; // Add new args
                len = j + 1; 
                

                if (n_flag && (num_args - base_args) == count) {
                    args[num_args] = 0; 
                    if (fork() == 0) {
                        exec(args[0], args); 
                        fprintf(2, "exec %s failed\n", args[0]); 
                        exit(1);
                    }
                    wait(0);
                    num_args = base_args; // Reset 
                }
            }
        }

        
        if (num_args > base_args) {
            args[num_args] = 0; // End list args
            if (fork() == 0) {
                exec(args[0], args); 
                fprintf(2, "exec %s failed\n", args[0]);
                exit(1);
            }
            wait(0);
        }
    }

    exit(0);
}