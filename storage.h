#ifndef STORAGE_H
#define STORAGE_H

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>


typedef struct QoraSQLData
{
    char* key;
    char* value;
} QoraSQLData;
 


void handle_select_command(int client_fd);
//void handle_insert_command(int client_fd,char* filename)


void create_file_from_db(int client_fd, char filename[256]);


#endif // STORAGE_H
