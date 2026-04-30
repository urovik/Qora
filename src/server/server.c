#include "server.h"
#include "core/core.h"
#include "core/utils.h"
#include "parser/sql_parser.h"
#include "core/logger.h"
#include "wrapper/wrappers.h"
#include "qoraLoop.h"
#include "qNetwork.h"


#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#define MAX_CLIENTS 10000





int run(int port) {
    qEventLoop *loop = qCreateLoop(MAX_CLIENTS);
    if (!loop) {
        fprintf(stderr, "Cannot create event loop\n");
        return -1;
    }

    // Создаём серверный сокет
    int listen_fd = listenServer(loop, SERVER_PORT);

    printf("Echo server started on port %d\n", port);
    qMain(loop);  // запуск event loop

    qDeleteLoop(loop);
    close(listen_fd);
    return 0;
}



   




