#include "ast.h"

#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>


Column* create_column(const char* name){
    Column* columnName = malloc(sizeof(Column));
    if (!columnName) {
        fprintf(stderr, "Memory allocation failed for Column\n");
        exit(1);
    }
    columnName->name = malloc(strlen(name));
    if(!columnName->name){
        fprintf(stderr, "Memory allocation failed for columnName\n");
        exit(1);
    }
    strcpy(columnName->name,name);
    columnName->next = NULL;
    return columnName;
    

}

Table* create_table(const char* name) {
    Table* tbl = malloc(sizeof(Table));
    if (!tbl) {
        fprintf(stderr, "Memory allocation failed for Table\n");
        exit(1);
    }
    tbl->name = malloc(strlen(name) + 1);
    if (!tbl->name) {
        fprintf(stderr, "Memory allocation failed for table name\n");
        free(tbl);
        exit(1);
    }
    strcpy(tbl->name, name);
    return tbl;
}

Expr* create_compare_expr(const char* column, Operator op, const char* value) {
    Expr* expr = malloc(sizeof(Expr));
    if (!expr) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    expr->type = EXPR_COMPARE;
    expr->compare.column = malloc(strlen(column) + 1);
    if (!expr->compare.column) { free(expr); exit(1); }
    strcpy(expr->compare.column, column);
    expr->compare.op = op;
    expr->compare.value = malloc(strlen(value) + 1);
    if (!expr->compare.value) { free(expr->compare.column); free(expr); exit(1); }
    strcpy(expr->compare.value, value);
    return expr;
}

Expr* create_binary_expr(ExprType type, Expr* left, Expr* right) {
    Expr* expr = malloc(sizeof(Expr));
    if (!expr) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    expr->type = type;
    expr->binary.left = left;
    expr->binary.right = right;
    return expr;
}

OrderBy* create_orderby(const char* column, int asc) {
    OrderBy* ob = malloc(sizeof(OrderBy));
    if (!ob) {
        fprintf(stderr, "Memory allocation failed for OrderBy\n");
        exit(1);
    }
    ob->column = malloc(strlen(column) + 1);
    if (!ob->column) {
        fprintf(stderr, "Memory allocation failed for order by column\n");
        free(ob);
        exit(1);
    }
    strcpy(ob->column, column);
    ob->asc = asc;
    return ob;
}

SelectStmt* create_select_stmt(Column* cols, Table* table, Expr* where, OrderBy* order_by) {
    SelectStmt* stmt = malloc(sizeof(SelectStmt));
    if (!stmt) {
        fprintf(stderr, "Memory allocation failed for SelectStmt\n");
        exit(1);
    }
    stmt->columns = cols;
    stmt->table = table;
    stmt->where = where;
    stmt->order_by = order_by;
    return stmt;
}