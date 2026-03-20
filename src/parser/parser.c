#include "parser.h"


#include <stdio.h>
#include <stdlib.h>



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

SelectStmt* parse_select(Parser* parser){
    return NULL;
}

Column* parse_columns(Parser* parser){
    return NULL;
}

Condition* parse_where(Parser* parser){
    return NULL;
}

OrderBy* parse_order_by(Parser* parser){
    return NULL;
}
