#ifndef CALLBACK_H
#define CALLBACK_H

#include "qoraLoop.h"
#include "client.h"

// Прототипы функций
void restart_client_timer(qEventLoop *eventLoop, Client *c);
int client_timeout(qEventLoop *eventLoop, int fd, void *clientData, int mask);


#endif // CALLBACK_H