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
SelectStmt* parse_select(Parser* parser); 
Column* parse_columns(Parser* parser);
Condition* parse_where(Parser* parser);
OrderBy* parse_order_by(Parser* parser);

#endif // PARSER_H