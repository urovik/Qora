#include "dict.h"
#include "core/utils.h"
#include "core/memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */


char* getValueFromDictByKey(Dict* storage, char* key){

    if(key == NULL){
        fprintf(stderr, "error: is not valid key");
        return NULL;
    }

    // избыточность в strlen нужно подумать как заменить избыточность плюч саму функциию она долгая
    size_t len_key = strlen(key);
    int hash_index = MurmurHash2(key, len_key);

    
    int index = hash_index % storage->capacity;
    DictNode* Node = storage->h_table[index];

    if(Node != NULL){
        // проходим по односвязному списку в поиске ключа
        while(Node != NULL){
            if(Node->key != NULL && strcmp(Node->key, key) == 0){
                return Node->value;
            }
            Node = Node->next;   
        }
    }

    return NULL;

}

int saveDataFromDict(Dict* storage, char* key, char* value){

    if(key == NULL && value == NULL){
        fprintf(stderr,"error: couldn't add value with this key");
        return -1;
    }


    size_t len_key = strlen(key);
    int hash_index = MurmurHash2(key, len_key);
    int index = hash_index % storage->capacity;

    // получаем по hash_index Node в словаре в которю будем складывать ключ и значение
    DictNode* Node = storage->h_table[index];

    // Ищем существующий ключ для обновления (или чтобы запретить перезапись)
    while(Node != NULL) {
        if(strcmp(Node->key, key) == 0) {
            // ключ уже есть: либо обновляем, либо возвращаем ошибку
            qfree(Node->value);
            Node->value = strdup(value);
            if(!Node->value) return -1;
            return 0;
        }
        Node = Node->next;
    }

    if(Node == NULL){
        DictNode* NewNode = qmalloc(sizeof(DictNode));
        NewNode->key = strdup(key);
        NewNode->value = strdup(value);
        if(!NewNode->key || !NewNode->value) {
        qfree(NewNode->key); qfree(NewNode->value); qfree(NewNode);
        return -1;
        }
        NewNode->next = NULL;
        storage->h_table[index] = NewNode;
    }

   


    return 0;

}
int deleteDataFromDict(Dict* storage);
int updateDataFromDict(Dict* storage);


