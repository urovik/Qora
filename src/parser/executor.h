#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser/ast.h"

void execute_select(SelectStmt* stmt, int client_fd);
void execute_create_table(const char* tablename, int client_fd);

#endif // EXECUTOR_H