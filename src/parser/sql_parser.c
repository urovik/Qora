#include "sql_parser.h"
#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>








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




