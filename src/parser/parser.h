#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"


typedef struct Parser{
    Lexer* lexer;
    Token currentTok;
} Parser;

typedef enum {
    PARSE_OK,
    PARSE_ERR_SYNTAX,
    PARSE_ERR_MEMORY,
    PARSE_ERR_UNEXPECTED_EOF,
} ParseResult;


void init_parser(Parser* parser, Lexer* lexer);

void next(Parser* parser);
ParseResult expect(Parser* parser, TokenType type);
 
SetAST* parse_set(Parser* parser);

#endif // PARSER_H