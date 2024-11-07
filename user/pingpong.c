#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void main(){
    int p[2];  
    pipe(p);   

    if (fork() == 0) {  
        char buf[1];
        read(p[0], buf, 1);  
        printf("%d: received ping\n", getpid());  
        write(p[1], buf, 1); 
        exit(0);
    } else {  
        char buf[1] = {'p'};
        write(p[1], buf, 1);  
        wait(0);             
        read(p[0], buf, 1);  
        printf("%d: received pong\n", getpid());  
        exit(0);
    }
}