#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>




void init_lexer(Lexer* lexer, const char* input_str){
    lexer->input_str = input_str;
    lexer->position = 0;
}

TokenType check_keyword(const char* word){
    if(strcasecmp(word,"SELECT") == 0) return SELECT;
    if(strcasecmp(word,"FROM") == 0) return FROM;
    if(strcasecmp(word,"WHERE") == 0) return WHERE;
    if(strcasecmp(word,"ORDER") == 0) return ORDER;
    if (strcasecmp(word, "BY") == 0) return BY;
    if (strcasecmp(word, "LIMIT") == 0) return LIMIT;
    if (strcasecmp(word, "TRUE") == 0) return TRUE;
    if (strcasecmp(word, "FALSE") == 0) return FALSE;
    if (strcasecmp(word, "NULL") == 0) return NULL_TOKEN;
    if (strcasecmp(word, "OR") == 0) return OR;
    if (strcasecmp(word, "AND") == 0) return AND; 
    if (strcasecmp(word, "NOT") == 0) return NOT;
    if (strcasecmp(word, "ASC") == 0) return ASC;
    if (strcasecmp(word, "DESC") == 0) return DESC;
    return IDENTIFIER;
}


Token next_token(Lexer* lexer){
    Token token;

    while(isspace(lexer->input_str[lexer->position])){
        lexer->position++;
    }

    if(lexer->input_str[lexer->position] == '\0'){
        token.token = EOF_TOKEN;
        token.text[0] = '\0'; 
        return token;
    }

    if(isdigit(lexer->input_str[lexer->position])){
        int n = 0;
        while(isdigit(lexer->input_str[lexer->position])){
            if (n < 63){
                token.text[n++] = lexer->input_str[lexer->position];
            }
            lexer->position++;
        }
        token.text[n] = '\0';
        token.token = NUMBER;
        return token;
    }
    
    if(isalpha(lexer->input_str[lexer->position]) || lexer->input_str[lexer->position] == '_'){
        int n = 0;
        while (isalnum(lexer->input_str[lexer->position]) || lexer->input_str[lexer->position] == '_')
        {
            if(n < 63){
                token.text[n++] = lexer->input_str[lexer->position];
            }
            lexer->position++;
        }

        token.text[n] = '\0';
        token.token = check_keyword(token.text);
        return token;
    }

    if(lexer->input_str[lexer->position] == '<' && lexer->input_str[lexer->position + 1] == '='){
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


    
    token.text[0] = lexer->input_str[lexer->position];
    token.text[1] = '\0';
    token.token = TOKEN_UNKNOWN;
    lexer->position++;

    return token;
    

}