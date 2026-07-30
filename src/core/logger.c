#include "logger.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>

    

/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */



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


void log_message(LogLevel level, const char* format, ...) {
    if(level < global_logger.level) return;
    if(global_logger.output_file == NULL) return;

    va_list args1, args2;
    va_start(args1, format);
    va_copy(args2, args1);  // Копируем для двойного использования

    time_t time_now = time(NULL);
    struct tm* tm_info = localtime(&time_now);
    
    char time_buffer[20];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", tm_info);

    const char* level_str = log_level_to_string(level);
    
    // Печатаем в файл
    fprintf(global_logger.output_file, "%s [%s]: ", time_buffer, level_str);
    vfprintf(global_logger.output_file, format, args1);
    fprintf(global_logger.output_file, "\n");
    fflush(global_logger.output_file);
    
    // Печатаем в консоль
    FILE* output = (level == LOG_LEVEL_ERROR || level == LOG_LEVEL_FATAL) ? stderr : stdout;
    fprintf(output, "%s [%s]: ", time_buffer, level_str);
    vfprintf(output, format, args2);
    fprintf(output, "\n");
    fflush(output);
    
    va_end(args1);
    va_end(args2);
}

void close_logger()
{
    if (global_logger.output_file != NULL)
    {
        fclose(global_logger.output_file);
        global_logger.output_file = NULL;
    }
}