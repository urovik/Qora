#ifndef QNETWORK_H
#define QNETWORK_H

#include "qoraLoop.h"


int listenServer(qEventLoop* evLoop, int port); // создает слущающий сокет

// следует добавить в acceptTcpHandler struct Clientdata, была еще маска но я убрал
void acceptTcpHandler(qEventLoop* evLoop,int listen_sock, void *clientData, int mask); // принимает входящие соединения на сокете 

// Clientdata пока что в разработке
void read_handler(qEventLoop *eventLoop, int fd, void *clientData, int mask); // когда маска Q_READABLE

void write_handler(qEventLoop *eventLoop, int fd, void *clientData, int mask); // когда маска Q_WRITABLE




#endif // QNETWORK_H
