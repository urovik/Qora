#include "storage.h"



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
        snprintf(response,sizeof(response),"Ошибка создания файла: %s\n",filename);
        write(client_fd,response,strlen(response));
        return;
    }


    
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





