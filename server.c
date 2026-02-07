#include "server.h"
#include "storage.h"



int create_server(int port){

    struct sockaddr_in address;
    int new_socket,epoll_fd;
    char filename[256];
    char response_message[8192];
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
                memset(response_message, 0, sizeof(response_message));
                snprintf(response_message, sizeof(response_message),
                        "Приветствую, ты подключился к Qorasql\n"
                        "Список команд:\n"
                        "SELECT (посмотреть файлы)\n"
                        "INSERT (добавить файл)\n"
                        "EXIT (отключиться)\n"
                        "CREATE (создать файл)\n");
                write(conn_fd, response_message, strlen(response_message));

                event.data.fd = conn_fd;
                event.events = EPOLLIN;
                if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,conn_fd,&event) == -1)
                {
                    perror("epoll_ctl(add client)");
                    close(conn_fd);
                }
            } else {
                

                // Данные от клиента
                char buffer[1024];
                ssize_t bytes_read = read(events[i].data.fd, buffer, sizeof(buffer));
                if(bytes_read <= 0)
                {
                    printf("Клиент отключился: fd=%d\n", events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                } 
                if (strncmp(buffer,"EXIT",4) == 0 || strncmp(buffer,"exit",4) == 0) {

                    
                    printf("Клиент отключился: fd=%d\n", events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                }
                 
                if(strncmp(buffer,"SELECT",6) == 0 || strncmp(buffer,"select",6) == 0 )
                {
                    // код SELECT
                    handle_select_command(events[i].data.fd);
                }

                if(strncmp(buffer,"CREATE",6) == 0 || strncmp(buffer,"create",6) == 0)
                {
                    memset(response_message, 0, sizeof(response_message));
                    snprintf(response_message,sizeof(response_message),"Введите имя файла для хранения данных не более 255 символов\n");
                    write(events[i].data.fd,response_message,sizeof(response_message));
                    read(events[i].data.fd,filename,sizeof(filename));
                    if(strlen(filename) > 255)
                    {
                        memset(response_message, 0, sizeof(response_message));
                        snprintf(response_message,sizeof(response_message),"Введите корректное имя файла");
                        write(events[i].data.fd,response_message,strlen(response_message));
                    }
                    create_file_from_db(events[i].data.fd,filename);
                }
                
                else {

                    // Выводим полученные данные
                    //printf("От клиента (fd=%d): %.*s\n", 
                    //       events[i].data.fd, (int)bytes_read, buffer);

                    // Отправляем ответ
                    //const char *response = "Сообщение получено!\n";
                    //write(events[i].data.fd, response, strlen(response));
                }
            }
        }
    }
    



    close(epoll_fd);
    close(socket_fd);

    return 0;


    /*if((new_socket = accept(socket_fd,(struct sockaddr*) &address, &addrlen)) == -1)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
        close(socket_fd);
    }*/


}

