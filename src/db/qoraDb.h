#ifndef QORADB_H
#define QORADB_H

#include "types.h"
#include "dict.h"

typedef struct{
    
    Dict* storage; 

} QoraDB;


QoraDB* createQoraDb(int size);
void freeQoraDb(QoraDB* qoraDB);


int set(QoraDB* qoraDB, char* key, char* value);
void delete(QoraDB* qoraDB, char* key);
char* get(QoraDB* qoraDB, char* key);

#endif // QORADB_H