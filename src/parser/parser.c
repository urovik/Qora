#include "parser.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Operator operator_from_token(Token tok){
    if(strcmp(tok.text,"=") == 0) return OP_EQ;
    if(strcmp(tok.text,"<") == 0) return OP_LT;
    if(strcmp(tok.text,">") == 0) return OP_GT;
    if(strcmp(tok.text,"<=") == 0) return OP_LE;
    if(strcmp(tok.text,">=") == 0) return OP_GE;
    if(strcmp(tok.text,"!=") == 0) return OP_NE;
    if(strcmp(tok.text,"<>") == 0) return OP_NE;
    fprintf(stderr, "Unknown operator: %s\n", tok.text);
    exit(1);
}



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



Column* parse_columns(Parser* parser){
    Column* head = NULL;
    Column* tail = NULL;

    while(1){
        if(parser->currentTok.token != IDENTIFIER){
            fprintf(stderr, "Syntax error: expected column name\n");
            exit(1);
        }
        if(parser->currentTok.token == STAR){
            next(parser);
            return NULL;
        }
        Column* col = create_column(parser->currentTok.text);
        if(head == NULL) head = tail = col;
        else{
            tail->next = col;
            tail = col;
        }

        next(parser);

        if(parser->currentTok.token == COMMA){
            next(parser);
            continue;
        } else{
            break;
        }
        
    }
    return head;
}

static Expr* parse_compare(Parser* parser){
    if(parser->currentTok.token != IDENTIFIER){
        fprintf(stderr, "Syntax error: expected column name in WHERE\n");
        exit(1);
    }
    char col[64];
    strcpy(col,parser->currentTok.text);
    next(parser);

    if(parser->currentTok.token != OPERATOR){
        fprintf(stderr, "Syntax error: expected operator in WHERE\n");
        exit(1);
    }
    Operator op = operator_from_token(parser->currentTok);
    next(parser);

    if (parser->currentTok.token != IDENTIFIER && parser->currentTok.token != NUMBER) {
        fprintf(stderr, "Syntax error: expected value in WHERE\n");
        exit(1);
    }
    char val[64];
    strcpy(val,parser->currentTok.text);
    next(parser);

    return create_compare_expr(col, op, val);

}

static Expr* parse_and_expr(Parser* parser){
    Expr* left = parse_compare(parser);
    while(parser->currentTok.token == AND){
        next(parser);
        Expr* right = parse_compare(parser);
        left = create_binary_expr(EXPR_AND, left, right);
    }
    return left;
}

Expr* parse_expr(Parser* parser){
    Expr* left = parse_and_expr(parser);
    while (parser->currentTok.token == OR){
        next(parser);
        Expr* right = parse_and_expr(parser);
        left = create_binary_expr(EXPR_OR, left, right);
    }
    return left;
    
}



OrderBy* parse_order_by(Parser* parser){
    if(parser->currentTok.token != IDENTIFIER){
        fprintf(stderr,"Syntax error: expected column name in orderBy");
        exit(1);
    }
    char col[64];
    strcpy(col,parser->currentTok.text);
    next(parser);

    int asc = 1; // по умолчанию
    if(parser->currentTok.token == ASC){
        asc = 1;
        next(parser);
    }
    else if(parser->currentTok.token == DESC){
        asc = 0;
        next(parser);
    }

    return create_orderby(col, asc);
}

SelectStmt* parse_select(Parser* parser){
    // SELECT
    if (parser->currentTok.token != SELECT) {
        fprintf(stderr, "Syntax error: expected SELECT\n");
        exit(1);
    }
    next(parser);

    // список колонок
    Column* cols = parse_columns(parser);

    // FROM
    if (parser->currentTok.token != FROM) {
        fprintf(stderr, "Syntax error: expected FROM\n");
        exit(1);
    }
    next(parser);

    // имя таблицы
    if (parser->currentTok.token != IDENTIFIER) {
        fprintf(stderr, "Syntax error: expected table name\n");
        exit(1);
    }
    Table* table = create_table(parser->currentTok.text);
    next(parser);

    // WHERE (необязательный)
    Expr* where = NULL;
    if (parser->currentTok.token == WHERE) {
        next(parser);
        where = parse_expr(parser);
    }

    // ORDER BY (необязательный)
    OrderBy* order_by = NULL;
    if (parser->currentTok.token == ORDER) {
        next(parser);
        if (parser->currentTok.token != BY) {
            fprintf(stderr, "Syntax error: expected BY after ORDER\n");
            exit(1);
        }
        next(parser);
        order_by = parse_order_by(parser);
    }

    // точка с запятой (необязательная)
    if (parser->currentTok.token == SEMICOLON) {
        next(parser);
    }

    // конец ввода
    if (parser->currentTok.token != EOF_TOKEN) {
        fprintf(stderr, "Syntax error: unexpected tokens after query\n");
        exit(1);
    }

    return create_select_stmt(cols, table, where, order_by);
}
