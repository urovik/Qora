#include "core.h"
#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

static int init_all_system_dir(){
    DIR* dir;
    const char* name_all_system_dir[] = {"base","qlogs"};
    size_t len_all_system_dir = sizeof(name_all_system_dir) / sizeof(name_all_system_dir[0]);
    char path_for_create[30];

    dir = opendir("./QORADATA");
    if(!dir){
        return -1;
    }
    closedir(dir);

    for(int i = 0; i < len_all_system_dir; i++){
        snprintf(path_for_create,sizeof(path_for_create),"./QORADATA/%s",name_all_system_dir[i]);
        if(access(path_for_create,F_OK) == -1){
            if(mkdir(path_for_create,0755) == -1){
                fprintf(stderr,strerror(errno));
                return -1;
            }

        }
        

    }
    
    return 0;
    
        
}



int init_system_dir(){

    const char* sys_dir_name = "QORADATA";
    if(access(sys_dir_name,F_OK) == -1){
        if(mkdir(sys_dir_name,0755) == 0){
            log_message(LOG_LEVEL_INFO,"системная директория QORADATA успешно создана");
            if(init_all_system_dir() == 0){
                log_message(LOG_LEVEL_INFO,"all system dir created");
                
            }else{
                return -1;
            }

            return 0;
        }
        else
        {
            log_message(LOG_LEVEL_FATAL,"system dir not created");
            fprintf(stderr,"%s",strerror(errno));
            return -1; 
        }
    }
    // создаем поддериктории даже если основная системная директория уже есть
    if(init_all_system_dir() == 0){
        log_message(LOG_LEVEL_INFO,"all system dir created");
                
    }
}


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





