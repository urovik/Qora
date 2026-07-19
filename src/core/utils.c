#include "utils.h"



/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */



int set_nonblocking_fd(int sockfd){

    int flags = fcntl(sockfd,F_GETFL,0);
    if (flags == -1){
        return -1;

    }
    flags |= O_NONBLOCK;
    if(fcntl(sockfd,F_SETFL,flags) == -1){
        return -1;
    }
    return 0;

}

// функция паники, для чрезвычайных ситуаций сервера, когда мы не можем дальше продолжать работу сервера 
void panic(const char* file, int line, const char* msg_err, ...){
    va_list ap;
    va_start(ap, msg_err);

    fprintf(stderr, "\nPANIC!!!\n");
    fprintf(stderr, "    File: %s:%d\n", file, line);
    fprintf(stderr, "    Message: ");
    vfprintf(stderr, msg_err, ap);
    fprintf(stderr, "\n\n");
    
    va_end(ap);
    abort();
}


// хэш функция
unsigned int MurmurHash2 ( char* key, unsigned int len)
{
  const unsigned int m = 0x5bd1e995;
  const unsigned int seed = 0;
  const int r = 24;

  unsigned int h = seed ^ len;

  const unsigned char * data = (const unsigned char *)key;
  unsigned int k;

  while (len >= 4)
    {
      k  = data[0];
      k |= data[1] << 8;
      k |= data[2] << 16;
      k |= data[3] << 24;

      k *= m;
      k ^= k >> r;
      k *= m;

      h *= m;
      h ^= k;

      data += 4;
      len -= 4;
    }

  switch (len)
    {
    case 3:
      h ^= data[2] << 16;
    case 2:
      h ^= data[1] << 8;
    case 1:
      h ^= data[0];
      h *= m;
    };

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}



int64_t get_monotonic_time_us(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts); // так получаем время в наносекундах нам надо микросекунды
    // tv_nsec — в наносекундах, делим на 1000 → получаем микросекунды
    return (int64_t)ts.tv_sec * 1000000LL + ts.tv_nsec / 1000;
}