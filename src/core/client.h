#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>

#include "db/qoraDb.h"


// структура клиента
typedef struct Client{
    int fd;
    char rbuf[1024];
    char wbuf[1024];
    QoraDB* db;
    int64_t last_activity_us; // время последей активности пользоавтателя

} Client;

#endif // CLIENT_H