#ifndef LEXER_H
#define LEXER_H


typedef enum {
    SET, EXPIRE, EOF_TOKEN,
    NUMBER, IN, UNDEFINED,TOKEN_UNKNOWN

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