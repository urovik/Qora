#ifndef SQL_PARSER_H
#define SQL_PARSER_H



#define LEN_SQL_COMMAND 8192


void execute_sql_command(const char sql_command[LEN_SQL_COMMAND]);
//  возможно понадобиться пока не понятно как void parse_sql_command();

void handle_select_command(int client_fd);
//void handle_insert_command(int client_fd,char* filename)

void handle_quit_command(int client_fd,int epoll_fd);


#endif // SQL_PARSER_H