#include "wrappers.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

ssize_t safe_write(int fd, const void *buf, size_t count) {
    ssize_t total_written = 0;
    const char *ptr = buf;
    while (total_written < (ssize_t)count) {
        ssize_t n = write(fd, ptr + total_written, count - total_written);
        if (n == -1) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Временная недоступность записи, возвращаем то, что записали
                return total_written;
            }
            perror("write failed");
            return -1;
        } else if (n == 0) {
            break;
        }
        total_written += n;
    }
    return total_written;
}
ssize_t safe_read(int fd, void *buf, size_t nbytes) {
    ssize_t total_read = 0;
    char *ptr = buf;
    while (total_read < (ssize_t)nbytes) {
        ssize_t n = read(fd, ptr + total_read, nbytes - total_read);
        if (n == -1) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Нет данных сейчас, возвращаем то, что прочитали (может быть 0)
                return total_read;
            }
            perror("read failed");
            return -1;
        } else if (n == 0) {
            break;
        }
        total_read += n;
    }
    return total_read;
}