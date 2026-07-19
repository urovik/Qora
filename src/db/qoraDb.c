#include <qoraDb.h>
#include <memory.h>
#include <stdlib.h>


/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */


QoraDB* createQoraDb(int size){

    QoraDB* db = (QoraDB*) qmalloc(sizeof(QoraDB));
    db->storage = (Dict*) qmalloc(sizeof(Dict));
    db->storage->h_table = qcalloc(size, sizeof(DictNode*));
    
    // вместимость хранилища 
    db->storage->capacity = (size_t) size;
    // текущая длина
    db->storage->size = 0;


    return db;


}

void freeQoraDb(QoraDB* qoraDB){
    if (!qoraDB || !qoraDB->storage) return;
    
    for (size_t i = 0; i < qoraDB->storage->capacity; ++i) {
        DictNode* node = qoraDB->storage->h_table[i];
        while (node != NULL) {
            DictNode* next = node->next;      // сохраняем следующий до free
            qfree(node->key);                 // или free, если key из strdup
            qfree(node->value);               // аналогично value
            qfree(node);                      // сама нода
            node = next;
        }
    }

    qfree(qoraDB->storage->h_table);
    qfree(qoraDB->storage);
    qfree(qoraDB);

}


int set(QoraDB* qoraDB, char* key, char* value){
    if (!qoraDB) return -1;
    return saveDataFromDict(qoraDB->storage, key, value);
    // сделать проверку на то что вернулось и поменять возращаемый тип на int

}

char* get(QoraDB* qoraDB, char* key){
    char* value = getValueFromDictByKey(qoraDB->storage, key);

    return value;

}