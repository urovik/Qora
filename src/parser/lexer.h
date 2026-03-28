#ifndef LEXER_H
#define LEXER_H


typedef enum {
    SELECT, FROM, WHERE, ORDER, BY,
    COMMA, SEMICOLON, OPERATOR, COLUMN,
    TABLE, LIMIT, TRUE, FALSE,
    NULL_TOKEN, OR, NOT, ASC, DESC,
    LPAREN, RPAREN, EOF_TOKEN, NUMBER, IDENTIFIER,
    TOKEN_UNKNOWN, AND,   

} TokenType;

typedef struct{
    TokenType token;
    char text[64];

} Token;

typedef struct{
    const char* input_str;
    int position;

} Lexer;



void init_lexer(Lexer* lexer, const char* input);
Token next_token(Lexer* lexer);


#endif // LEXER_H