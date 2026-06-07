#ifndef TYPES_H
#define TYPES_H

// использование strlen() O(n)

#include <stdio.h>

typedef struct DictNode DictNode;

struct DictNode
{
    char* key;
    char* value; // пока что только строка в планах сделать любой тип
    DictNode* next; // односвязный список для исключения коллизий

};


typedef struct{

    size_t size;
    size_t capacity;
    DictNode** h_table; // массив key-value значений


} Dict;


#endif // TYPES_H