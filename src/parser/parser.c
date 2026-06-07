#include "parser.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void expect(Parser* parser, TokenType type){
    if(parser->currentTok.token == type){
        next(parser);
    } else{
        fprintf(stderr,"Syntax error: expected token type %d, got %d (%s)\n", type, parser->currentTok.token, parser->currentTok.text);
        exit(1);
    }
}

SetAST* parse_set(Parser* parser){
    if(parser->currentTok.token != SET){
        fprintf(stderr, "Syntax error: expected SET\n");
        return NULL;
    }
    // получаем value
    next(parser);

    char* value = parser->currentTok.text;

    // получаем IN
    next(parser);

    if(parser->currentTok.token != IN){
        fprintf(stderr, "Syntax error: expected IN\n");
        return NULL;
    }
    // получаем key
    next(parser);

    char* key = parser->currentTok.text;

    return create_set(key,value,0);
    
}

