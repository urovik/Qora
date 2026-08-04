#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <errno.h>


#include "core.h"
#include "utils.h"
#include "qoraLoop.h"
#include "qNetwork.h"
#include "wrappers.h"




/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */


void read_handler(qEventLoop *loop, int fd, void *data, int mask) {
    (void)mask;
    Client *c = (Client*)data;
    if (!c) { close(fd); return; }
    c->last_activity_us = get_monotonic_time_us();

    // перезапускаем таймер клиента, чтобы отсчет времени начался заново
    restart_client_timer(loop, c);


    ssize_t n = safe_read(fd, c);
    if (n <= 0) {

        if(c->timer_id >= 0){
            cancel_timer(loop, c->timer_id);
            c->timer_id = -1;
        } 

        if (n == CLIENT_INVALID || n == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
            qDeleteFileEvent(loop, fd, Q_READABLE | Q_WRITABLE);
            close(fd);
            qfree(c->rbuf);
            qfree(c);
            printf("Client disconnected: fd=%d\n", fd);
        }
        return;
    }

    // ОБРАБАТЫВАЕМ ВСЕ КОМАНДЫ В БУФЕРЕ
    while (c->total_amount_rbuf > 0) {
        // Ищем конец команды
        char *newline = memchr(c->rbuf, '\n', c->total_amount_rbuf);
        if (!newline) {
            // Нет полной команды, ждем еще данных
            printf("No complete command, waiting for more data\n");
            break;
        }
        
        size_t cmd_len = newline - c->rbuf + 1;
        char *command = c->rbuf;
        
        printf("Processing command: %.*s", (int)cmd_len, command);
        
        // Обрабатываем команду
        int processed = 0;
        
        if (strncmp(command, "SELECT ", 7) == 0) {
            int idx = atoi(command + 7);
            if (idx >= 0 && idx < g_server->dbnum) {
                c->db = g_server->dbs[idx];
                safe_write(fd, "OK switched\r\n", 13);
            } else {
                safe_write(fd, "-ERR invalid DB\r\n", 16);
            }
            processed = 1;
        }
        else if (strncmp(command, "SET ", 4) == 0) {
            char *sp = strchr(command + 4, ' ');
            if (!sp) { 
                safe_write(fd, "-ERR syntax\r\n", 12); 
            } else {
                *sp = '\0';
                char *key = command + 4;
                char *value = sp + 1;
                value[strcspn(value, "\r\n")] = '\0';
                
                int rc = set(c->db, key, value);
                if (rc == 0) {
                    safe_write(fd, "+OK\r\n", 5);
                    printf("SET OK: %s = %s\n", key, value);
                } else {
                    safe_write(fd, "-ERR set failed\r\n", 15);
                }
            }
            processed = 1;
        }
        else if (strncmp(command, "GET ", 4) == 0) {
            char *key = command + 4;
            key[strcspn(key, "\r\n")] = '\0';
            
            char *val = get(c->db, key);
            if (val) {
                char resp[4096];
                size_t len = snprintf(resp, sizeof(resp), "$%zu\r\n%s\r\n", 
                                     strlen(val), val);
                safe_write(fd, resp, len);
                printf("GET OK: %s = %s\n", key, val);
            } else {
                safe_write(fd, "$-1\r\n", 5);
                printf("GET not found: %s\n", key);
            }
            processed = 1;
        }
        
        // Если команда не обработана - неизвестная
        if (!processed) {
            safe_write(fd, "-ERR unknown command\r\n", 22);
            printf("Unknown command: %.*s", (int)cmd_len, command);
        }
        
        // УДАЛЯЕМ ОБРАБОТАННУЮ КОМАНДУ ИЗ БУФЕРА
        if (cmd_len < c->total_amount_rbuf) {
            memmove(c->rbuf, c->rbuf + cmd_len, 
                    c->total_amount_rbuf - cmd_len);
        }
        c->total_amount_rbuf -= cmd_len;
        printf("Buffer now has %zu bytes\n", c->total_amount_rbuf);
    }
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
    // Создаём клиента
    Client *c = qmalloc(sizeof(Client));
    if (!c) { close(client_fd); panic("malloc client"); }

    c->fd = client_fd;
    memset(c->wbuf, 0, sizeof(c->wbuf));

    c->size_rbuf = SIZE_RBUF_CLIENT;
    c->rbuf = qmalloc(c->size_rbuf);
    if (!c->rbuf) { close(client_fd); perror("malloc rbuf not allocated"); }
    c->total_amount_rbuf = 0;
    c->size_wbuf = 2048;
    c->total_amount_wbuf = 0;
    

    // Привязываем к первой БД из пула
    c->db = g_server->dbs[0];
    // получаем время последней активности клиента
    c->last_activity_us = get_monotonic_time_us();

    // Регистрируем чтение; clientData = клиент, чтобы в read_handler был доступ к c->db
    qCreateFileEvent(evLoop, client_fd, Q_READABLE, read_handler, c);

    
    printf("accept new client fd = %d\n", client_fd);

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