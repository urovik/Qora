#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>

int set_nonblocking_fd(int sockfd);
void panic(const char* file, int line, const char* msg_err, ...);

unsigned int MurmurHash2 (char* key, unsigned int len);

int64_t get_monotonic_time_us(void);
#endif // UTILS_H
