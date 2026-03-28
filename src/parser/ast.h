#ifndef AST_H
#define AST_H

typedef enum{
    OP_GE,
    OP_LE,
    OP_NE,
    OP_EQ,
    OP_LT,
    OP_GT,
} Operator;

typedef enum Expr_type{
    EXPR_COMPARE,
    EXPR_AND,
    EXPR_OR,
} ExprType;

typedef struct Column{
    char* name;
    struct Column* next;
} Column;

typedef struct Table{
    char* name;
} Table;


typedef struct Expr{
    ExprType type;
    union {
        struct {
            char* column;
            Operator op;
            char* value;
        } compare;

        struct {
            struct Expr* left;
            struct Expr* right;
        } binary;     
    };
} Expr;


typedef struct OrderBy{
    char* column;
    int asc; 
} OrderBy;

typedef struct SelectStmt{
    Column* columns;
    Table* table;
    Expr* where;
    OrderBy* order_by;

} SelectStmt;
 
Column* create_column(const char* name);
Table* create_table(const char* name);
Expr* create_compare_expr(const char* column, Operator op, const char* value);
Expr* create_binary_expr(ExprType type, Expr* left, Expr* right);
OrderBy* create_orderby(const char* column, int asc);
SelectStmt* create_select_stmt(Column* cols, Table* table, Expr* where, OrderBy* order_by);

void add_column(Column** list, Column* new_col);

#endif // AST_H