#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <unistd.h>
#include <core/client.h>


#define MAX_TRY 100

ssize_t safe_write(int fd, const void *wbuf, size_t count);
ssize_t safe_read(int fd, Client *c);


#endif // WRAPPERS_H