#include "sql_parser.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/epoll.h>


void execute_sql_command(const char sql_command[LEN_SQL_COMMAND])
{
    if (strlen(sql_command) > LEN_SQL_COMMAND){
        perror("Команда SQL слишком длинная");
        return;
    } 
    // в работу вступает парсер SQL
}

// перенести в отдельный файлик
void handle_quit_command(int client_fd,int epoll_fd){
    // добавить лог что клиент отключился
    epoll_ctl(epoll_fd,EPOLL_CTL_DEL,client_fd,NULL);
    close(client_fd);
}

void handle_select_command(int client_fd)
{
    DIR* dir;
    struct dirent* entry;
    char response[8192] = "";
    int offset = 0;


    dir = opendir(".");
    if(!dir)
    {
        snprintf(response,sizeof(response),"выбранная директория не найдена\n");
        write(client_fd,response,strlen(response));
        return;
    }

    while((entry = readdir(dir)) != NULL)
    {
        if(strcmp(entry->d_name,".") != 0 && (strcmp(entry->d_name,"..")) != 0)
        {
            int len = snprintf(response + offset,sizeof(response) - offset,"%s\n",entry->d_name);
            if(len < 0 || offset + len >= sizeof(response)){
                closedir(dir);
                write(client_fd,"Ошибка: буфер переполнен\n",25);
                return;
            }
            offset += len;
        }   
    }
    
    closedir(dir);

    if(offset == 0){
        strcpy(response,"Директория пуста\n");
    }

    write(client_fd,response,strlen(response));
}
