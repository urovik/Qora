#include "parser/ast.h" 
#include "wrapper/wrappers.h"
#include "executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void execute_select(SelectStmt* stmt, int client_fd) {
    if (!stmt->columns) {
        safe_write(client_fd, "Error: no columns specified\n", 28);
        return;
    }

    char table_path[512];
    // Используем имя таблицы из stmt->table
    snprintf(table_path, sizeof(table_path), "/qora/QORADATA/base/%s", stmt->table->name);

    FILE* file = fopen(table_path, "r");
    if (!file) {
        safe_write(client_fd, "Error: table not found\n", 23);
        return;
    }

    // Если выбраны все колонки (SELECT *)
    if (stmt->columns == NULL) {
        char line[4096];
        while (fgets(line, sizeof(line), file)) {
            safe_write(client_fd, line, strlen(line));
        }
        fclose(file);
        return;
    }

    // Выбор конкретных колонок (пока не реализовано)
    safe_write(client_fd, "Specific column selection not supported yet\n", 44);
    fclose(file);
}

void execute_create_table(const char* tablename, int client_fd) {
    char path[512];
    snprintf(path, sizeof(path), "/qora/QORADATA/base/%s", tablename);
    FILE* f = fopen(path, "w");
    if (!f) {
        safe_write(client_fd, "Error creating table\n", 20);
        return;
    }
    safe_write(client_fd, "Table created\n", 14);
    fclose(f);
}