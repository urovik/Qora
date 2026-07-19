#include "parser.h"
#include "errorExec.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */


/*static Operator operator_from_token(Token tok){
    if(strcmp(tok.text,"=") == 0) return OP_EQ;
    if(strcmp(tok.text,"<") == 0) return OP_LT;
    if(strcmp(tok.text,">") == 0) return OP_GT;
    if(strcmp(tok.text,"<=") == 0) return OP_LE;
    if(strcmp(tok.text,">=") == 0) return OP_GE;
    if(strcmp(tok.text,"!=") == 0) return OP_NE;
    if(strcmp(tok.text,"<>") == 0) return OP_NE;
    fprintf(stderr, "Unknown operator: %s\n", tok.text);
    exit(1);
}*/





void init_parser(Parser* parser, Lexer* lexer){
    parser->lexer = lexer;
    parser->currentTok = next_token(lexer);
}

void next(Parser* parser){
    parser->currentTok = next_token(parser->lexer);
}

ParseResult expect(Parser* parser, TokenType type){
    if(parser->currentTok.token == type){
        next(parser);
    } else{
        fprintf(stderr,"Syntax error: expected token type %d, got %d (%s)\n", type, parser->currentTok.token, parser->currentTok.text);
        return PARSE_ERR_SYNTAX;
    }
}

SetAST* parse_set(Parser* parser){
    if(expect(parser, SET) == PARSE_ERR_SYNTAX){
        //set_err(&err, parser->currentTok.line, parser->currentTok.column, "expect SET") // нужно переделать логику ошибки сделать va аргументы
        return NULL; // не лучший вариант но пока так
    }          // если не SET — ошибка и exit; если да — уже перешли дальше

    char* value_raw = parser->currentTok.text;

    next(parser);                 // переходим к следующему токену (IN)

    if(expect(parser, IN) == PARSE_ERR_SYNTAX){
        return NULL;

    }           
    char* key_raw = parser->currentTok.text;

    char* key = strdup(key_raw);
    char* value = strdup(value_raw);

    if (!key || !value) {
        free(key); free(value);
        return NULL; // или вернуть ошибку памяти
    }

    // нужно реализовать проверку что дальше после value нет мусора, иначе при следующем чтение у нас в lexer останется этот мусор

    SetAST* ast = create_set(key, value, 0);         // create_set не должен делать ещё один strdup
    if (!ast) {
        free(key); free(value);
        return NULL;
    }
    return ast;
}

