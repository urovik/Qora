#ifndef ERROREXEC_H
#define ERROREXEC_H

#include <stdlib.h>
#include <stdio.h>


typedef struct {
    int line, column;
    char message[256];
} Error;
// глобальная переменная err 

extern Error err;

inline void set_err(Error* err, int line, int col, const char* m);




#endif // ERROREXEC_H