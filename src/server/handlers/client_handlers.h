#ifndef CLIENT_HANDLERS_H
#define CLIENT_HANDLERS_H

#include <stddef.h> 

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



int init_client_pool(int initial_size, client_t** clients, int* max_clients);
int expand_client_pool(client_t** clients, int* max_clients);
void free_client(int client_idx, int epoll_fd, client_t* clients, int max_clients);
int find_or_create_client(int conn_fd, client_t** clients, int* max_clients);

int find_client_index(int fd, client_t* clients, int max_clients);

void handle_client(client_t* client, int epoll_fd, int client_idx, client_t* clients, int max_clients);






#endif // CLIENT_HANDLERS_H