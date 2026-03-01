#include "logger.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

    


Logger global_logger;

const char* log_level_to_string(LogLevel level)
    {
         switch (level)
        {
        case LOG_LEVEL_DEBUG: return "DEBUG"; 
        case LOG_LEVEL_INFO: return "INFO";
        case LOG_LEVEL_WARNING: return "WARN"; 
        case LOG_LEVEL_ERROR: return "ERROR"; 
        case LOG_LEVEL_FATAL: return "FATAL"; 
        default: return "UNKNOWN"; 
        }
    }

void init_logger(const char* output_filename,LogLevel level)
    {
        global_logger.level = level;
        global_logger.output_file = fopen(output_filename,"a");
        if(global_logger.output_file == NULL)
        {
            perror("Error init logger");
            exit(EXIT_FAILURE);
            
        }
    }


void log_message(LogLevel level,char* message)
    {
        if(level < global_logger.level) return;
        if(global_logger.output_file == NULL) return;

        time_t time_now = time(NULL);
        struct tm* tm_info = localtime(&time_now);
        
        char time_buffer[20];
        const char* format = "%Y-%m-%d %H:%M:%S";
        strftime(time_buffer, sizeof(time_buffer), format, tm_info);

        const char* level_str = log_level_to_string(level);
        if (level_str == NULL) {
            level_str = "UNKNOWN";
        }
        if (level == LOG_LEVEL_ERROR || level == LOG_LEVEL_FATAL){
            fprintf(global_logger.output_file,"%s [%s]: %s\n",time_buffer,level_str,message);
            fprintf(stderr,"%s [%s]: %s\n",time_buffer,level_str,message);
            fflush(global_logger.output_file);
            return;
        }

        fprintf(global_logger.output_file,"%s [%s]: %s\n",time_buffer,level_str,message);
        fprintf(stdout,"%s [%s]: %s\n",time_buffer,level_str,message);
        fflush(global_logger.output_file);
        return;
    }


void close_logger()
{
    if (global_logger.output_file != NULL)
    {
        fclose(global_logger.output_file);
        global_logger.output_file = NULL;
    }
}