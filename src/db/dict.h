#ifndef DICT_H
#define DICT_H

#include <types.h>

// нужны бы свои ответы, в идеале задуматься о своем протоколе qpro

char* getValueFromDictByKey(Dict* storage, char* key);

int saveDataFromDict(Dict* storage, char* key, char* value);
int deleteDataFromDict(Dict* storage);
int updateDataFromDict(Dict* storage);




#endif // DICT_H