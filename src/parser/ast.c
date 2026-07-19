#include "ast.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */


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

    setAST->key = key;
    setAST->value = value;

    return setAST;
    
    
}

void freeSetAST(SetAST* ast){
    qfree(ast->key);
    qfree(ast->value);
    qfree(ast);

}