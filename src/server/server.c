#include "server.h"
#include "core/core.h"
#include "core/utils.h"
#include "parser/sql_parser.h"
#include "core/logger.h"
#include "wrapper/wrappers.h"
#include "qoraLoop.h"


#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#define MAX_CLIENTS 10000


// Обработчик чтения данных от клиента (эхо)
static void read_handler(qEventLoop *loop, int fd, void *data, int mask) {
    (void)data; (void)mask;
    char buf[4096];
    ssize_t n = safe_read(fd, buf, sizeof(buf));
    if (n <= 0) {
        if (n == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
            // Клиент закрыл соединение или ошибка
            qDeleteFileEvent(loop, fd, Q_READABLE | Q_WRITABLE);
            close(fd);
            printf("Client disconnected: fd=%d\n", fd);
        }
        return;
    }
    // Отправляем данные обратно (эхо)
    safe_write(fd, buf, n);
    // Для демонстрации можно также добавить обработку записи,
    // но здесь эхо отправляется сразу в том же вызове.
}


// Обработчик входящего соединения (accept)
static void accept_handler(qEventLoop *loop, int fd, void *data, int mask) {
    (void)data; (void)mask;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int client_fd = accept(fd, (struct sockaddr*)&client_addr, &addrlen);
    if (client_fd == -1) {
        perror("accept");
        return;
    }
    set_nonblocking_fd(client_fd);
    // Регистрируем событие чтения для нового клиента
    qCreateFileEvent(loop, client_fd, Q_READABLE, read_handler, NULL);
    printf("New client connected: fd=%d\n", client_fd);
}


int run(int port) {
    qEventLoop *loop = qCreateLoop(MAX_CLIENTS);
    if (!loop) {
        fprintf(stderr, "Cannot create event loop\n");
        return -1;
    }

    // Создаём серверный сокет
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket");
        return -1;
    }
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };
    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(listen_fd);
        return -1;
    }
    if (listen(listen_fd, 128) == -1) {
        perror("listen");
        close(listen_fd);
        return -1;
    }
    set_nonblocking_fd(listen_fd);

    // Регистрируем событие чтения на слушающем сокете
    qCreateFileEvent(loop, listen_fd, Q_READABLE, accept_handler, NULL);

    printf("Echo server started on port %d\n", port);
    qMain(loop);  // запуск event loop

    qDeleteLoop(loop);
    close(listen_fd);
    return 0;
}



   




