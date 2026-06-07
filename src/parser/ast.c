#include "ast.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


SetAST* create_set(char *key, char* value, int TTL){
    // придумать что делать с паникой в qmalloc
    SetAST* setAST = qmalloc(sizeof(SetAST));
    if(!setAST){
        fprintf(stderr,"Memory allocation failed for SetAST");
        return NULL;
    }

    if(TTL == 0){
        setAST->TTL = 20;
    }

    setAST->key = strdup(key);
    setAST->value = strdup(value);

    return setAST;
    
    
}