#ifndef SERVER_H
#define SERVER_H

#include <stddef.h> 

#define SERVER_PORT 5960
#define MAX_EVENTS 10
#define MAX_CLIENTS 10

#define MAX_CMD_LEN 256
#define SQL_QUERY_LEN 8192
#define OUT_BUF 4096

typedef struct client_t{
    int client_fd;
    char sql_str[SQL_QUERY_LEN];
    size_t len_sql_str;
    char command_buffer[MAX_CMD_LEN];
    size_t len_command_buffer;
    char out_buf[OUT_BUF];
    size_t len_out_buf;

} client_t;

int start_server(int port);

static void handle_client(client_t* client,int epoll_fd);   



#endif // SERVER_H