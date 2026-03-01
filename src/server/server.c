#include "server.h"
#include "core/core.h"
#include "core/utils.h"
#include "parser/sql_parser.h"
#include "core/logger.h"


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

    

int start_server(int port){

    struct sockaddr_in address;
    int new_socket,epoll_fd;
    
    // Инициализируем пул клиентов при запуске сервера
    if (init_client_pool(MAX_CLIENTS, &clients, &max_clients) == -1) {
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
    log_message(LOG_LEVEL_INFO,"сервер запущен на порту 5960");

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

                
                
                int client_idx = find_or_create_client(conn_fd, &clients, &max_clients);

                
                if (client_idx == -1) {
                    write(conn_fd, "Ошибка сервера\n", 15);
                    close(conn_fd);
                    continue;
                }

                
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
                    free_client(client_idx,epoll_fd, clients, max_clients);
                    }
            
            } else{
                int client_idx = find_client_index(events[i].data.fd, clients, max_clients);
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
                    free_client(client_idx,epoll_fd, clients, max_clients);
                    continue;
                }

                if (events[i].events & EPOLLIN) {
                    handle_client(&clients[client_idx], epoll_fd, client_idx, clients, max_clients);
                }
                
            }
                  
    }
    
    //закрываем while
    }

    

    // Очистка памяти перед завершением
    for (int i = 0; i < max_clients; i++) {
        if (clients[i].client_fd != 0) {
            close(clients[i].client_fd);
        }
    }

    free(clients);
    clients = NULL;
    max_clients = 0;

    close(epoll_fd);
    close(socket_fd);

    return 0;
}


   




