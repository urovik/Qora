#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "logger.h"


int main(int argc, char *argv[]){

    init_logger(QORA_LOGS_FILENAME,LOG_LEVEL_DEBUG);
    log_message(LOG_LEVEL_INFO,"Сервер запущен");
    start_server(SERVER_PORT);

    close_logger();


    return 0;
}