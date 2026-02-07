#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define QORA_LOGS_FILENAME "qora_logs.txt"



typedef enum {
    LOG_LEVEL_DEBUG, 
    LOG_LEVEL_INFO, 
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LogLevel;


typedef struct 
{
    FILE* output_file;
    LogLevel level; 
} Logger;

extern Logger global_logger;

void init_logger(const char* output_filename,LogLevel level);
void log_message(LogLevel level,char* message);

const char* log_level_to_string(LogLevel level);

void close_logger();





#endif // LOGGER_H