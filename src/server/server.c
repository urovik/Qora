#include "server.h"
#include "core/core.h"
#include "core/utils.h"
#include "core/logger.h"
#include "wrapper/wrappers.h"
#include "qoraLoop.h"
#include "qNetwork.h"
#include "db/qoraDb.h"


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

    // инициализируем наше key-value хранилище 
    QoraDB* DB = createQoraDb(100);
    if(!DB){
        panic("storage is not initializied");
    }

    /*set(DB,"name", "Юра");
    char *val = get(DB,"name");
    printf("%s",val);
    */
    qEventLoop *loop = qCreateLoop(MAX_CLIENTS);
    if (!loop) {
        panic("cannot create evloop");
    }

    // Создаём серверный сокет
    int listen_fd = listenServer(loop, SERVER_PORT);

    printf("Echo server started on port %d\n", port);
    qMain(loop);  // запуск event loop

    freeQoraDb(DB);
    qDeleteLoop(loop);
    close(listen_fd);
    return 0;
}



   




