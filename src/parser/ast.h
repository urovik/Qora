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

typedef struct Column{
    char* name;
    struct Column* next;
} Column;

typedef struct Table{
    char* name;
} Table;


typedef struct Condition{
    char* column;
    Operator op;
    char* value;
} Condition;


typedef struct OrderBy{
    char* column;
    int asc; 
} OrderBy;

typedef struct SelectStmt{
    Column* columns;
    Table* table;
    Condition* where;
    OrderBy* order_by;

} SelectStmt;
 
Column* create_column(const char* name);
Table* create_table(const char* name);
Condition* create_condition(const char* column, Operator op, const char* value);
OrderBy* create_orderby(const char* column, int asc);
SelectStmt* create_select_stmt(Column* cols, Table* table, Condition* where, OrderBy* order_by);

void add_column(Column** list, Column* new_col);

#endif // AST_H