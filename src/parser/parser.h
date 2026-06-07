#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"


typedef struct Parser{
    Lexer* lexer;
    Token currentTok;
} Parser;


void init_parser(Parser* parser, Lexer* lexer);

void next(Parser* parser);
void expect(Parser* parser, TokenType type);
 
SetAST* parse_set(Parser* parser);

#endif // PARSER_H