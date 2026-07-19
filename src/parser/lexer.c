#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>


/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */


void init_lexer(Lexer* lexer, const char* input_str){
    lexer->input_str = input_str;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
}


static char peek_char(const Lexer* lexer) {
    return lexer->input_str[lexer->position];
}


static void advance_lexer(Lexer* lexer) {
    char c = lexer->input_str[lexer->position];
    lexer->position++;
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;   // сброс колонки в начало строки
    } else {
        lexer->column++;      // иначе просто двигаемся вправо
    }
}


static TokenType check_keyword(const char* word){
    if(strcasecmp(word,"SET") == 0) return SET;
    if(strcasecmp(word, "EXPIRE") == 0) return EXPIRE;
    if(strcasecmp(word, "IN") == 0) return IN;
    return UNDEFINED;
    
}


Token next_token(Lexer* lexer) {
    Token token;

    // Позиция начала токена
    token.line = lexer->line;
    token.column = lexer->column;

    // Пропуск пробелов: сначала проверка на \0, потом isspace
    while (peek_char(lexer) != '\0' && isspace(peek_char(lexer))) {
        advance_lexer(lexer);
    }

    char c = peek_char(lexer);

    // Конец ввода
    if (c == '\0') {
        token.token = EOF_TOKEN;
        token.text[0] = '\0';
        return token;
    }

    // Цифры
    if (isdigit(c)) {
        int n = 0;
        while (peek_char(lexer) != '\0' && isdigit(peek_char(lexer)) && n < 63) {
            token.text[n++] = peek_char(lexer);
            advance_lexer(lexer);
        }
        token.text[n] = '\0';
        token.token = NUMBER;
        return token;
    }

    // Идентификаторы / ключевые слова
    if (isalpha(c) || c == '_') {
        int n = 0;
        while (peek_char(lexer) != '\0'
               && (isalnum(peek_char(lexer)) || peek_char(lexer) == '_')
               && n < 63)
        {
            token.text[n++] = peek_char(lexer);
            advance_lexer(lexer);
        }
        token.text[n] = '\0';
        token.token = check_keyword(token.text);
        return token;
    }


    /*if(lexer->input_str[lexer->position] == '<' && lexer->input_str[lexer->position + 1] == '='){
        token.text[0] = '<';
        token.text[1] = '=';
        token.text[2] = '\0';
        token.token = OPERATOR;
        lexer->position += 2;
        return token;
    }

    if(lexer->input_str[lexer->position] == '>' && lexer->input_str[lexer->position + 1] == '='){
        token.text[0] = '>';
        token.text[1] = '=';
        token.text[2] = '\0';
        token.token = OPERATOR;
        lexer->position += 2;
        return token;
    }

    if(lexer->input_str[lexer->position] == '!' && lexer->input_str[lexer->position + 1] == '='){
        token.text[0] = '!';
        token.text[1] = '=';
        token.text[2] = '\0';
        token.token = OPERATOR;
        lexer->position += 2;
        return token;
    }

    if(lexer->input_str[lexer->position] == '<' && lexer->input_str[lexer->position + 1] == '>'){
        token.text[0] = '<';
        token.text[1] = '>';
        token.text[2] = '\0';
        token.token = OPERATOR;
        lexer->position += 2;
        return token;
    }

    if(lexer->input_str[lexer->position] == '<'){
        token.text[0] = '<';
        token.text[1] = '\0';
        token.token = OPERATOR;
        lexer->position++;
        return token;
    } 

    if(lexer->input_str[lexer->position] == '>'){
        token.text[0] = '>';
        token.text[1] = '\0';
        token.token = OPERATOR;
        lexer->position++;
        return token;
    }

    if(lexer->input_str[lexer->position] == '='){
        token.text[0] = '=';
        token.text[1] = '\0';
        token.token = OPERATOR;
        lexer->position++;
        return token;
    }

    if(lexer->input_str[lexer->position] == ';'){
        token.text[0] = ';';
        token.text[1] = '\0';
        token.token = SEMICOLON;
        lexer->position++;
        return token;
    }

    if(lexer->input_str[lexer->position] == ','){
        token.text[0] = ',';
        token.text[1] = '\0';
        token.token = COMMA;
        lexer->position++;
        return token;
    }

    if(lexer->input_str[lexer->position] == '('){
        token.text[0] = '(';
        token.text[1] = '\0';
        token.token = LPAREN;
        lexer->position++;
        return token;
    }

    if(lexer->input_str[lexer->position] == ')'){
        token.text[0] = ')';
        token.text[1] = '\0';
        token.token = RPAREN;
        lexer->position++;
        return token;
    }

    if(lexer->input_str[lexer->position] == '*'){
        token.text[0] = '*';
        token.text[1] = '\0';
        token.token = STAR;
        lexer->position++;
        return token;
    }
    */

    
    // Любой другой одиночный символ
    token.text[0] = c;
    token.text[1] = '\0';
    token.token = TOKEN_UNKNOWN;
    advance_lexer(lexer);

    return token;

    return token;
    

}