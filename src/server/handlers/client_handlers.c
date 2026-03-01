#include "client_handlers.h"
#include "core/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>

int init_client_pool(int initial_size, client_t** clients, int* max_clients) {
    *clients = malloc(initial_size * sizeof(client_t));
    if (!*clients) {
        fprintf(stderr, "malloc: %s\n", strerror(errno));
        return -1;
    }
    *max_clients = initial_size;
    for (int i = 0; i < *max_clients; i++) {
        (*clients)[i].client_fd = -1;
    }
    return 0;
}

int expand_client_pool(client_t** clients, int* max_clients) {
    int old_max = *max_clients;
    *max_clients += 10;
    client_t* temp = realloc(*clients, *max_clients * sizeof(client_t));
    if (!temp) {
        fprintf(stderr, "realloc: %s\n", strerror(errno));
        return -1;
    }
    *clients = temp;

    // Инициализируем новые слоты
    for (int i = old_max; i < *max_clients; i++) {
        (*clients)[i].client_fd = -1;
        (*clients)[i].len_command_buffer = 0;
        memset((*clients)[i].command_buffer, 0, MAX_CMD_LEN);
        memset((*clients)[i].out_buf, 0, OUT_BUF);
    }
    return 0;
}

void free_client(int client_idx, int epoll_fd, client_t* clients, int max_clients) {
    if (client_idx < 0 || client_idx >= max_clients) {
        return;
    }

    int fd = clients[client_idx].client_fd;
    if (fd <= 0) {
        return; // Уже закрыт или невалидный
    }

    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        perror("epoll_ctl DEL");
    }
    close(fd);

    clients[client_idx].client_fd = -1; // Помечаем как закрытый

    fprintf(stdout, "Клиент %d (fd=%d) корректно отключился\n", client_idx, fd);

    // Очищаем буфер
    clients[client_idx].len_command_buffer = 0;
    memset(clients[client_idx].command_buffer, 0, MAX_CMD_LEN);
    memset(clients[client_idx].out_buf, 0, OUT_BUF);
}

int find_or_create_client(int conn_fd, client_t** clients, int* max_clients) {
    // Ищем свободный слот с client_fd == -1
    for (int i = 0; i < *max_clients; i++) {
        if ((*clients)[i].client_fd == -1) {
            (*clients)[i].client_fd = conn_fd;
            (*clients)[i].len_command_buffer = 0;
            memset((*clients)[i].command_buffer, 0, MAX_CMD_LEN);
            memset((*clients)[i].out_buf, 0, OUT_BUF);
            fprintf(stdout, "Создан новый клиент: индекс=%d, fd=%d\n", i, conn_fd);
            return i;
        }
    }

    // Свободных слотов нет — расширяем пул
    int old_max = *max_clients;
    if (expand_client_pool(clients, max_clients) == -1) {
        return -1;
    }

    // Берём первый из новых слотов
    (*clients)[old_max].client_fd = conn_fd;
    (*clients)[old_max].len_command_buffer = 0;
    memset((*clients)[old_max].command_buffer, 0, MAX_CMD_LEN);
    memset((*clients)[old_max].out_buf, 0, OUT_BUF);
    fprintf(stdout, "Создан новый клиент: индекс=%d, fd=%d\n", old_max, conn_fd);
    return old_max;
}

int find_client_index(int fd, client_t* clients, int max_clients) {
    for (int j = 0; j < max_clients; j++) {
        if (clients[j].client_fd == fd && clients[j].client_fd > 0) {
            return j;
        }
    }
    return -1;
}

void handle_client(client_t* client, int epoll_fd, int client_idx, client_t* clients, int max_clients) {
    ssize_t bytes_read;

    while (1) {
        ssize_t free_space = MAX_CMD_LEN - client->len_command_buffer - 1;
        if (free_space <= 0) {
            log_message(LOG_LEVEL_ERROR, "Буфер переполнен");
            client->len_command_buffer = 0;
            continue;
        }

        bytes_read = read(
            (client->client_fd),
            (client->command_buffer),
            free_space
        );

        if (bytes_read > 0) {
            client->len_command_buffer += bytes_read;
            size_t cmd_len = strlen(client->command_buffer);

            char* cmd_end = memchr(client->command_buffer, '\n', client->len_command_buffer);
            if (cmd_end == NULL) break;

            *cmd_end = '\0';

            if (client->command_buffer[0] != 0) {
                if (strncmp(client->command_buffer, "/dt", 3) == 0 && cmd_len == 4) {
                    snprintf(client->out_buf, sizeof(client->out_buf), "таблицы...\n");
            write(client->client_fd, client->out_buf, strlen(client->out_buf));
                } else {
                    snprintf(client->out_buf, sizeof(client->out_buf), "Введена неизвестная команда\n");
            write(client->client_fd, client->out_buf, strlen(client->out_buf));
                }
            } else {
                snprintf(client->out_buf, sizeof(client->out_buf), "Пустая команда\n");
                write(client->client_fd, client->out_buf, strlen(client->out_buf));
            }
            client->len_command_buffer = 0;
            memset(client->out_buf, 0, OUT_BUF);
            memset(client->command_buffer, 0, MAX_CMD_LEN);
        }

        if (bytes_read == 0) {
            free_client(client_idx, epoll_fd, clients, max_clients);
            return;
        }

        if (bytes_read == -1) {
            if (errno == EINTR) {
                continue; // Повторить read()
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            perror("Ошибка чтения");
            return;
        }
    }
}
