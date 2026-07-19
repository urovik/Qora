#include "server.h"
#include "core/core.h"
#include "core/utils.h"
#include "core/logger.h"
#include "wrapper/wrappers.h"
#include "qoraLoop.h"
#include "qNetwork.h"
#include "memory.h"



#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */



int run(int port) {

    // инициализируем наши key-value хранилище 
    Qserver server = {0};
    server.dbnum = MAX_DB;

    // аллоцируем массив указателей (сами БД пока не созданы)
    server.dbs = qcalloc(server.dbnum, sizeof(QoraDB*));
    if (!server.dbs) panic("cannot alloc dbs array");

    // создаём каждую БД отдельно
    for (int i = 0; i < server.dbnum; ++i) {
        server.dbs[i] = createQoraDb(MAX_SIZE_DB);
        if (!server.dbs[i]) {
            // откат: освобождаем уже созданные
            for (int j = 0; j < i; ++j) freeQoraDb(server.dbs[j]);
            qfree(server.dbs);
            panic("cannot create db %d", i);
        }
    }

    
    qEventLoop *loop = qCreateLoop(MAX_CLIENTS);
    if (!loop) {
        panic("cannot create evloop");
    }
    
    // Создаём серверный сокет
    int listen_fd = listenServer(loop, SERVER_PORT);

    printf("Echo server started on port %d\n", port);

    // ВАЖНО: нужно как-то протащить server в обработчики.
    // Самый чистый способ: сделать глобальный указатель на сервер
    // или хранить его внутри loop. Для старта проще глобальный.
    extern Qserver *g_server;
    g_server = &server;
    
    qMain(loop);  // запуск event loop

     // очистка
    for (int i = 0; i < server.dbnum; ++i) {
        freeQoraDb(server.dbs[i]);
    }
    qDeleteLoop(loop);
    close(listen_fd);
    return 0;
}



   




