#ifndef CORE_H
#define CORE_H

#include <utils.h>

// макрос для паники
#define panic(...) panic(__FILE__,__LINE__, __VA_ARGS__)


typedef struct QoraSQLData
{
    char* key;
    char* value;
} QoraSQLData;
 

void create_file_from_db(int client_fd, char filename[256]);
static int init_all_system_dir();
int init_system_dir();


#endif // CORE_H
