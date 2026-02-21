#include "core.h"

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>


typedef struct QoraSQLData
{
    char* key;
    char* value;
} QoraSQLData;


void create_file_from_db(int client_fd, char filename[256])
{
    DIR* dir;
    struct dirent* entry;
    char response[8192] = "";
    FILE* new_file;
    char path_to_create_file[1024];

    dir = opendir("./QORADATA");

    if(!dir)
    {
        snprintf(response,sizeof(response),"выбранная директория не найдена\n");
        write(client_fd,response,strlen(response));
        return;
    }
    closedir(dir);
    snprintf(path_to_create_file,sizeof(path_to_create_file),"./QORADATA/%s",filename);
    new_file = fopen(path_to_create_file,"w+");

    if(new_file != NULL)
    {
        snprintf(response,sizeof(response),"файл: %s cоздан\n",filename);
        write(client_fd,response,strlen(response));
        fclose(new_file);
        return;
    } 
    else{
        snprintf(response,sizeof(response),"Ошибка создания файла: %s\nОтправьте команду повторно\n",filename);
        write(client_fd,response,strlen(response));
        return;
    }


    
}





