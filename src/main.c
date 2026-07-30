#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server/server.h"
#include "core/logger.h"
#include "core/core.h"


/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */



int main(int argc, char *argv[]){
    (void)argc;
    (void)argv;

    init_logger(QORA_LOGS_FILENAME,LOG_LEVEL_DEBUG);
    if(init_system_dir() == -1){
        exit(EXIT_FAILURE);
    } 
    log_message(LOG_LEVEL_INFO,"Сервер запущен");
    run(SERVER_PORT);

    close_logger();


    return 0;
}