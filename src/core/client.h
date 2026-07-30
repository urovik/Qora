#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "db/qoraDb.h"

#define MAX_SIZE_CLIENT_BUFFER 100 * 1024 * 1024 // 100 mb // qora.conf
#define SIZE_RBUF_CLIENT 4096 // 4KB
#define SIZE_WBUF_CLIENT 4096 // 4KB

#define CLIENT_INVALID 0 // возращаем когда клиент злоупотребляет памятью 

// структура клиента
typedef struct Client{
    int fd;
    char* rbuf;
    size_t total_amount_rbuf; // текущие количество элементов в массиве rbuf
    size_t size_rbuf;
    char wbuf[2048];
    size_t total_amount_wbuf; // текущие количество элементов в массиве wbuf
    size_t size_wbuf;
    QoraDB* db;
    int64_t last_activity_us; // время последей активности пользоавтателя
    int timer_id; // идентификатор таймера для клиента

} Client;

int expand_rbuf(Client *c);

void smart_shrink_rbuf(Client *c);

#endif // CLIENT_H