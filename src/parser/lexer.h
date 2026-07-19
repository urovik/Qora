#ifndef LEXER_H
#define LEXER_H


typedef enum {
    SET, EXPIRE, EOF_TOKEN,
    NUMBER, IN, UNDEFINED,TOKEN_UNKNOWN

} TokenType;

typedef struct{
    TokenType token;
    char text[64];
    int line;
    int column;

} Token;

typedef struct{
    const char* input_str;
    int position;
    int line;
    int column;

} Lexer;



void init_lexer(Lexer* lexer, const char* input);
static char peek_char(const Lexer* lexer);
static void advance_lexer(Lexer* lexer);
Token next_token(Lexer* lexer);


#endif // LEXER_H