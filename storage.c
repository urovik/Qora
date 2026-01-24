#include "storage.h"



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
        if(strcmp(entry->d_name,".") == 0 || (strcmp(entry->d_name,"..")) == 0)
        {
            continue;
        }   
    
    int len = snprintf(response + offset,sizeof(response) - offset,"%s\n",entry->d_name);
    if(len < 0 || offset + len >= sizeof(response)){
        closedir(dir);
        write(client_fd,"Ошибка: буфер переполнен\n",25);
        return;
        }

    offset += len;
    }
    
    closedir(dir);

    if(offset = 0){
        strcpy(response,"Директория пуста\n");
    }

    write(client_fd,response,strlen(response));
}





