#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char *argv[]){


    FILE* file = fopen("my_db.csv","r");

    char buffer[1024];

    while(fgets(buffer,sizeof(buffer),file)){
        char* data = strchr(buffer,'\n');
        if (data) *data = '\0';

        char* name = strtok(buffer,";");
        char* surname = strtok(NULL,";");


        if (name && surname)
        {
            printf("Имя: %s, фамилия: %s\n",name,surname);

        }
        else
        {
            printf("некорректная строка: %s\n",buffer);
        }
     } 


    fclose(file);


    return 0;
}