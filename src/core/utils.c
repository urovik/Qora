#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>


int set_nonblocking_fd(int sockfd){

    int flags = fcntl(sockfd,F_GETFL,0);
    if (flags == -1){
        return -1;

    }
    flags |= O_NONBLOCK;
    if(fcntl(sockfd,F_SETFL,flags) == -1){
        return -1;
    }
    return 0;

}

// функция паники, для чрезвычайных ситуаций сервера, когда мы не можем дальше продолжать работу сервера 
void panic(const char* file, int line, const char* msg_err, ...){
    va_list ap;
    va_start(ap, msg_err);

    fprintf(stderr, "\nPANIC!!!\n");
    fprintf(stderr, "    File: %s:%d\n", file, line);
    fprintf(stderr, "    Message: ");
    vfprintf(stderr, msg_err, ap);
    fprintf(stderr, "\n\n");
    
    va_end(ap);
    abort();
}



