#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <errno.h>


#include "core.h"
#include "utils.h"
#include "qoraLoop.h"
#include "qNetwork.h"
#include "wrappers.h"


// Обработчик чтения данных от клиента (эхо)
void read_handler(qEventLoop *loop, int fd, void *data, int mask) {
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

void acceptTcpHandler(qEventLoop* evLoop,int listen_sock, void *clientData, int mask){
    struct sockaddr_in addr_client;
    socklen_t addrlen = sizeof(addr_client);

    int client_fd = accept(listen_sock,(struct sockaddr*) &addr_client, &addrlen);
    if(client_fd == -1){
        perror("error accept client");
        return;
    }

    set_nonblocking_fd(client_fd);

    qCreateFileEvent(evLoop, client_fd, Q_READABLE, read_handler, NULL);
    printf("accept new client fd = %d", client_fd);

}

// делаю функцию void так ка использую макрос panic
int listenServer(qEventLoop* evLoop, int port){

    struct sockaddr_in addr;
    int listen_fd;

    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        panic("failed make listen_fd");
        return -1; // недостижимый return -1, делается чтобы убрать предупреждения
    }
    // код ниже делается для того чтобы при перезапуске не получить ошибку что порт занят
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if(bind(listen_fd, (struct sockaddr*) &addr, sizeof(addr)) == -1){
        // не вызываем close(), так как panic() сделает все за нас
        panic("failed bind listen_fd");
        return -1; // недостижимый return
    }
    if(listen(listen_fd,4096) == -1){
        panic("failed listen");
    }
    // делаем слущающий сокет неблокирующим
    set_nonblocking_fd(listen_fd);

    // Регистрируем событие чтения на слушающем сокете
    qCreateFileEvent(evLoop, listen_fd, Q_READABLE, acceptTcpHandler, NULL);
    return listen_fd;
    
}