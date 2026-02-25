#include "server.h"
#include "core.h"
#include "utils.h"
#include "sql_parser.h"
#include "logger.h"


#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static client_t* clients = NULL;


static int max_clients = MAX_CLIENTS;


static int init_client_pool(int initial_size) {
    clients = malloc(initial_size * sizeof(client_t));
    if (!clients) {
        fprintf(stderr, "malloc: %s\n", strerror(errno));
        return -1;
    }
    max_clients = initial_size;
    for (int i = 0; i < max_clients; i++) {
        clients[i].client_fd = -1;
    }
    return 0;
}

static int expand_client_pool() {
    int old_max = max_clients;
    max_clients += 10;
    client_t* temp = realloc(clients, max_clients * sizeof(client_t));
    if (!temp) {
        fprintf(stderr, "realloc: %s\n", strerror(errno));
        return -1;
    }
    clients = temp;

    // Инициализируем новые слоты
    for (int i = old_max; i < max_clients; i++) {
        clients[i].client_fd = -1;
        clients[i].len_command_buffer = 0;
        memset(clients[i].command_buffer, 0, MAX_CMD_LEN);
        memset(clients[i].out_buf, 0, OUT_BUF);
    }
    return 0;
}

static void free_client(int client_idx,int epoll_fd) {
    if (client_idx < 0 || client_idx >= max_clients) {
        return;
    }

    int fd = clients[client_idx].client_fd;
    if (fd <= 0) {
        return; // Уже закрыт или невалидный
    }

    // Удаляем fd из epoll перед закрытием
    // Удаляем из epoll перед закрытием
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        perror("epoll_ctl DEL");
    }
    close(fd);

    clients[client_idx].client_fd = -1; // Помечаем как закрытый

    fprintf(stdout, "Клиент %d (fd=%d) корректно отключился\n", client_idx, fd);

    // Очищаем буфер
    clients[client_idx].len_command_buffer = 0;
    memset(clients[client_idx].command_buffer, 0, MAX_CMD_LEN);
    memset(clients[client_idx].out_buf, 0, OUT_BUF);
}

static int find_or_create_client(int conn_fd) {
    // Сначала ищем свободный слот с client_fd == -1 (корректно закрытый)
    // Поиск свободного слота во всём массиве
    for (int i = 0; i < max_clients; i++) {
        if (clients[i].client_fd == -1) {
            clients[i].client_fd = conn_fd;
            clients[i].len_command_buffer = 0;
            memset(clients[i].command_buffer, 0, MAX_CMD_LEN);
            memset(clients[i].out_buf, 0, OUT_BUF);
            fprintf(stdout, "Создан новый клиент: индекс=%d, fd=%d\n", i, conn_fd);
            return i;
        }
    }

    // Свободных слотов нет — расширяем пул
    int old_max = max_clients;
    if (expand_client_pool() == -1)
        return -1;

    // Новые слоты уже инициализированы client_fd = -1 в expand_client_pool
    // Берём первый из них
    clients[old_max].client_fd = conn_fd;
    clients[old_max].len_command_buffer = 0;
    memset(clients[old_max].command_buffer, 0, MAX_CMD_LEN);
    memset(clients[old_max].out_buf, 0, OUT_BUF);
    fprintf(stdout, "Создан новый клиент: индекс=%d, fd=%d\n", old_max, conn_fd);
    return old_max;
}

static int find_client_index(int fd) {
    for (int j = 0; j < max_clients; j++) {
        if (clients[j].client_fd == fd && clients[j].client_fd > 0) {
            return j;
        }
    }
    return -1;
}


static void handle_client(client_t* client, int epoll_fd,int client_idx) {
    ssize_t bytes_read;
    char* end_symbol;
    
    while (1)
    {
    
        ssize_t free_space = MAX_CMD_LEN - client->len_command_buffer - 1;
        if (free_space <= 0) {
            log_message(LOG_LEVEL_ERROR, "Буфер переполнен");
            client->len_command_buffer = 0;
            continue;  
        }
        
        
        bytes_read = read(
            client->client_fd,
            client->command_buffer,
            free_space
        );

        if (bytes_read > 0) {
            client->len_command_buffer += bytes_read;

            size_t cmd_len = strlen(client->command_buffer);

            char* cmd_end = memchr(client->command_buffer, '\n', client->len_command_buffer);
            if (cmd_end == NULL) break; 

            
            *cmd_end = '\0';
            
            if (client->command_buffer[0] != 0) {
                
                if (strncmp(client->command_buffer, "/dt",3) == 0 && cmd_len == 4) {
                    snprintf(client->out_buf, sizeof(client->out_buf), "таблицы...\n");
                    write(client->client_fd, client->out_buf, strlen(client->out_buf));
                }
                else {
                    snprintf(client->out_buf, sizeof(client->out_buf), "Введена неизвестная команда\n");
                    write(client->client_fd, client->out_buf, strlen(client->out_buf));
                }
                }
            else {
                
                snprintf(client->out_buf, sizeof(client->out_buf), "Пустая команда\n");
                write(client->client_fd, client->out_buf, strlen(client->out_buf));
            }
            client->len_command_buffer = 0; 
            memset(client->out_buf,0,OUT_BUF); 
            memset(client->command_buffer,0,MAX_CMD_LEN);
        }

        
        
        
            

        
        if(bytes_read == 0) {
            //printf("Клиент fd = %d отключился\n",client->client_fd);
            //handle_quit_command(client->client_fd,epoll_fd);
            //client->client_fd = 0;
            //client->len_command_buffer = 0;
            free_client(client_idx,epoll_fd);
            return;    
            }
    
        if(bytes_read == -1){
            if (errno == EINTR) {
                continue;  // Повторить read()
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            perror("Ошибка чтения");
            return;
            }
        }
    }



    

int start_server(int port){

    struct sockaddr_in address;
    int new_socket,epoll_fd;
    
    // Инициализируем пул клиентов при запуске сервера
    if (init_client_pool(MAX_CLIENTS) == -1) {
        exit(EXIT_FAILURE);
    }

    socklen_t addrlen = sizeof(address);

    struct epoll_event event, events[MAX_EVENTS];
    

    int socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if (socket_fd == -1)
    {
        perror("ошибка сервера");
        exit(EXIT_FAILURE);
        
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(socket_fd,(struct sockaddr*) &address,sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
        
    }

    if(listen(socket_fd, 4096) == -1)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
        
    }


    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll socket");
        exit(EXIT_FAILURE);
        
    } 

    event.data.fd = socket_fd;
    event.events = EPOLLIN; // EPOLLIN флаг который значит данные готовые для чтения 
    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,socket_fd,&event) == -1)
    {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    } 
    printf("сервер запущен на порту %d\n",port);

    while (1)
    {
        int nfds = epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
        if (nfds == -1) {
            perror("epoll_wait");
            continue;
        }

        // обрабатываем все готовые файловые дескрипторы
        for(int i = 0; i < nfds; i++)
        {
            if(events[i].data.fd == socket_fd)
            {
                int conn_fd = accept(socket_fd,NULL,NULL);
                if (conn_fd == -1) {
                    perror("accept");
                    continue;
                }

                
                
                int client_idx = find_or_create_client(conn_fd);

                
                if (client_idx == -1) {
                    write(conn_fd, "Ошибка сервера\n", 15);
                    close(conn_fd);
                    continue;
                }
                // Инициализируем клиента
                //clients[client_idx].client_fd = conn_fd;
                //clients[client_idx].len_command_buffer = 0;

                
                char response_message[8192];
                memset(response_message, 0, sizeof(response_message));
                snprintf(response_message, sizeof(response_message),
                        "Приветствую, ты подключился к Qorasql\n"
                        //"Список команд:\n"
                        //"SELECT (посмотреть файлы)\n"
                        //"INSERT (добавить файл)\n"
                        //"EXIT (отключиться)\n"
                        //"CREATE (создать файл)\n"
                        );
                write(conn_fd, response_message, strlen(response_message));

                int is_nonblock = set_nonblocking_fd(conn_fd);
                if (is_nonblock == -1){
                    perror("error set nonblock");
                    close(conn_fd);
                    continue;   
                } 

                event.data.fd = conn_fd;
                event.events = EPOLLIN | EPOLLET;
                if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,conn_fd,&event) == -1)
                    {
                    perror("epoll_ctl(add client)");
                    close(conn_fd);
                    free_client(client_idx,epoll_fd);
                    }
            
            } else{
                int client_idx = find_client_index(events[i].data.fd);
                if (client_idx == -1) {
                    fprintf(stderr, "Неизвестный fd=%d, пропускаем\n", events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    continue;
                }

                // Проверяем, что клиент ещё активен
                if (clients[client_idx].client_fd <= 0) {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    continue;
                }

                if (events[i].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
                    fprintf(stderr, "Получено событие отключения для клиента %d (fd=%d)\n", client_idx, events[i].data.fd);
                    free_client(client_idx,epoll_fd);
                    continue;
                }

                if (events[i].events & EPOLLIN) {
                    handle_client(&clients[client_idx], epoll_fd, client_idx);
                }
                
            }
                  
    }
    
    //закрываем while
    }

    close(epoll_fd);
    close(socket_fd);

    // Очистка памяти перед завершением
    for (int i = 0; i < max_clients; i++) {
        if (clients[i].client_fd != 0) {
            close(clients[i].client_fd);
        }
    }

    free(clients);
    clients = NULL;
    max_clients = 0;

    return 0;
}


   




