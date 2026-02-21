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





static void handle_client(client_t* client, int epoll_fd) {
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
        
            char* cmd_end = memchr(client->command_buffer, '\n', client->len_command_buffer);
            if (cmd_end == NULL) break; 

            
            *cmd_end = '\0';
            
            if (client->command_buffer[0] != 0) {
                
                if (strncmp(client->command_buffer, "/dt",3) == 0 && strlen(client->command_buffer) == 4) {
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
            printf("Клиент fd = %d отключился",client->client_fd);
            handle_quit_command(client->client_fd,epoll_fd);
            client->client_fd = 0;
            client->len_command_buffer = 0;
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
            handle_quit_command(client->client_fd, epoll_fd);
            return;
            }
        }
    }



    
    



 
        


   


int start_server(int port){

    struct sockaddr_in address;
    int new_socket,epoll_fd;
    
    static client_t clients[MAX_CLIENTS] = {0};

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

    if(listen(socket_fd, 128) == -1)
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
                int client_idx = -1;
                for(int j = 0; j < MAX_CLIENTS; j++) {
                    if(clients[j].client_fd == 0){
                        client_idx = j;
                        break;
                    }
                }
                if (client_idx == -1) {
                    write(conn_fd, "Сервер переполнен\n", 16);
                    close(conn_fd);
                    continue;
                }
                // Инициализируем клиента
                clients[client_idx].client_fd = conn_fd;
                clients[client_idx].len_command_buffer = 0;

                
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
                    clients[client_idx].client_fd = 0;
                    continue;
                } 

                event.data.fd = conn_fd;
                event.events = EPOLLIN | EPOLLET;
                if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,conn_fd,&event) == -1)
                    {
                    perror("epoll_ctl(add client)");
                    close(conn_fd);
                    clients[client_idx].client_fd = 0;
                    }
            
            } else{
                
                int client_idx = -1;
                for(int j = 0; j < MAX_CLIENTS; j++) {
                    if(clients[j].client_fd == events[i].data.fd){
                        client_idx = j;
                        break;
                    }
                }
                if(client_idx == -1){
                    continue;
                }

                

                handle_client(&clients[client_idx], epoll_fd);
                // Данные от клиента
                    
                // все это заменить функцией 
                /*if (strncmp(buffer,"EXIT",4) == 0 || strncmp(buffer,"exit",4) == 0) {

                    
                    printf("Клиент отключился: fd=%d\n", events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd); 
                    }
                 
                if(strncmp(buffer,"SELECT",6) == 0 || strncmp(buffer,"select",6) == 0)
                    {
                    // код SELECT
                    handle_select_command(events[i].data.fd);
                    }

                if(strncmp(buffer,"CREATE",6) == 0 || strncmp(buffer,"create",6) == 0)
                    {
                    memset(response_message, 0, sizeof(response_message));
                    snprintf(response_message,sizeof(response_message),"Введите имя файла для хранения данных не более 255 символов\n");
                    write(events[i].data.fd,response_message,sizeof(response_message));

                    // перед тем как читать filename сбрасываем его чтобы там не остались лишние ланные после ошибки
                    memset(filename,0,sizeof(filename));
                    ssize_t n = read(events[i].data.fd,filename,sizeof(filename) - 1);
                    
                    /*if (errno == EAGAIN || errno == EWOULDBLOCK) {
                         continue;  // Нет данных сейчас
                    }
                    filename[n] = '\0';
                    if(strlen(filename) > 255)
                        {
                        memset(response_message, 0, sizeof(response_message));
                        snprintf(response_message,sizeof(response_message),"Введите корректное имя файла\n");
                        write(events[i].data.fd,response_message,strlen(response_message));
                        }
                    create_file_from_db(events[i].data.fd,filename);
                    } */

                    //else {

                    // Выводим полученные данные
                    //printf("От клиента (fd=%d): %.*s\n", 
                    //       events[i].data.fd, (int)bytes_read, buffer);

                    // Отправляем ответ
                    //const char *response = "Сообщение получено!\n";
                    //write(events[i].data.fd, response, strlen(response));
                    //}
            }
                  
    }
    
    //закрываем while
    }
// закрываем функцию

    
    
    
    
    close(epoll_fd);
    close(socket_fd);

    return 0;
}


   




