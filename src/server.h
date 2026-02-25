#ifndef SERVER_H
#define SERVER_H

#include <stddef.h> 

#define SERVER_PORT 5960
#define MAX_EVENTS 10
#define MAX_CLIENTS 10

#define MAX_CMD_LEN 256
#define SQL_QUERY_LEN 4096
#define OUT_BUF 512

typedef struct client_t{
    int client_fd;
    char sql_str[SQL_QUERY_LEN];
    size_t len_sql_str;
    char command_buffer[MAX_CMD_LEN];
    size_t len_command_buffer;
    char out_buf[OUT_BUF];
    size_t len_out_buf;

} client_t;


static void handle_client(client_t* client,int epoll_fd,int client_idx);  
static int init_client_pool(int initial_size);
static int expand_client_pool(); 
static void free_client(int client_idx,int epoll_fd);

static int find_client_index(int fd);

static int find_or_create_client(int conn_fd);

int start_server(int port);




#endif // SERVER_H