#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <unistd.h>

#define MAX_TRY 100

ssize_t safe_write(int fd, const void *buf, size_t count);
ssize_t safe_read(int fd, void *buf, size_t nbytes);


#endif // WRAPPERS_H