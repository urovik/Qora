#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server/server.h"
#include "core/logger.h"
#include "core/core.h"



int main(int argc, char *argv[]){

    init_logger(QORA_LOGS_FILENAME,LOG_LEVEL_DEBUG);
    if(init_system_dir() == -1){
        exit(EXIT_FAILURE);
    } 
    log_message(LOG_LEVEL_INFO,"Сервер запущен");
    start_server(SERVER_PORT);

    close_logger();


    return 0;
}