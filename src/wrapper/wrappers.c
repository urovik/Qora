#include "wrappers.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "core/logger.h"
#include "core/client.h"

ssize_t safe_write(int fd, const void *wbuf, size_t count) {
    ssize_t total_written = 0;
    const char *ptr = wbuf;
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
ssize_t safe_read(int fd, Client *c) {
    ssize_t total_read = 0;

    if (c->size_rbuf >= MAX_SIZE_CLIENT_BUFFER) {
        log_message(LOG_LEVEL_ERROR, "Client buffer exceeded max limit: %zu", c->size_rbuf);
        return CLIENT_INVALID; // Сообщаем вызывающему коду, что нужно закрыть клиента
    }

    size_t free_space = c->size_rbuf - c->total_amount_rbuf;

    if (free_space == 0) {
        // вызовем увелечениe буфера
        if(expand_rbuf(c) == -1) {
            log_message(LOG_LEVEL_ERROR, "Failed to expand rbuf for client fd=%d\n", fd);
            perror("Failed to expand rbuf for client fd");
            return -1;
        }
        if(c->size_rbuf > MAX_SIZE_CLIENT_BUFFER){
            return CLIENT_INVALID;
        }
        free_space = c->size_rbuf - c->total_amount_rbuf;
    }

    while (total_read < (ssize_t)free_space) {
        ssize_t n = read(fd, c->rbuf + c->total_amount_rbuf + total_read, free_space - total_read);
        if (n == -1) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Нет данных сейчас, возвращаем то, что прочитали (может быть 0)
                break;
            }
            perror("read failed");
            return -1;
        } else if (n == 0) {
            c->total_amount_rbuf += total_read;
            break;
        }
        total_read += n;
        c->total_amount_rbuf += n;       
    }
    smart_shrink_rbuf(c);
    return total_read;
}