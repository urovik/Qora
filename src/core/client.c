#include "client.h"
#include "core/logger.h"
/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */



int expand_rbuf(Client *c){

    c->size_rbuf = c->size_rbuf * 2;
    char *new_rbuf = realloc(c->rbuf, c->size_rbuf);

    if(!new_rbuf){
        perror("Failed to expand rbuf");
        return -1;
    }
    c->rbuf = new_rbuf;
    return 0;
}

void smart_shrink_rbuf(Client *c){
    // не сжимаем если буфер маленький
    if(c->size_rbuf <= SIZE_RBUF_CLIENT * 2){
        return;
    }

    // Сжимаем только если использовано < 25%
    double usage = (double)c->total_amount_rbuf / c->size_rbuf;
    if (usage > 0.25) {
        return;
    }

    size_t old_size = c->size_rbuf;
    c->size_rbuf = c->size_rbuf / 2;
    char *new_rbuf = realloc(c->rbuf, c->size_rbuf);
    if(!new_rbuf){
        perror("Failed to expand rbuf");
        return;
    }

    c->rbuf = new_rbuf;
    log_message(LOG_LEVEL_INFO,"Smart shrink: %zu -> %zu\n", old_size, c->size_rbuf);
}