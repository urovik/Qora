#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>



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



