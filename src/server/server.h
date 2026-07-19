#ifndef SERVER_H
#define SERVER_H

#include "db/qoraDb.h"


#define SERVER_PORT 5960
#define MAX_CLIENTS 10000
#define MAX_EVENTS 10
#define MAX_DB 3
#define MAX_SIZE_DB 100


typedef struct Qserver
{

    QoraDB** dbs;
    int dbnum;

} Qserver;





int run(int port);




#endif // SERVER_H