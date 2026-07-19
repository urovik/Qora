#include "executor.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */


int execute_set_value(QoraDB* qoraDb, Parser* parser){
    

    // парсим в SetAST с помощью парсера через parse_set
    SetAST* setAst = parse_set(parser);
    if(setAst == NULL){
        return -1;
    }


    // вызываем set
    if(set(qoraDb, setAst->key, setAst->value) == -1){
        freeSetAST(setAst);
        return -1;
    }

    // возращаем что то типо SET_OK
    return 0;
}