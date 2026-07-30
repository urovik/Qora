#include "callback.h"
#include "core.h"
#include "utils.h"
#include "wrappers.h"
#include "timers.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void restart_client_timer(qEventLoop *eventLoop, Client *c) {
    if (!eventLoop || !c) return;
    
    // Отменяем старый таймер, если он был
    if (c->timer_id >= 0) {
        cancel_timer(eventLoop, c->timer_id);
        c->timer_id = -1;
    }
    
    // Создаем новый таймер с задержкой CLIENT_TIMEOUT_MS (30000 мс)
    c->timer_id = add_timer(eventLoop, CLIENT_TIMEOUT_MS, client_timeout, c);
    
    printf("Timer restarted for client fd=%d, timer_id=%d, delay=%dms\n", 
           c->fd, c->timer_id, CLIENT_TIMEOUT_MS);
}

int client_timeout(qEventLoop *eventLoop, int fd, void *clientData, int mask) {
    (void)mask;
    (void)fd;
    
    Client *c = (Client*)clientData;
    if (!c) return 0;

    // Проверяем, жив ли клиент
    if (!c || c->fd < 0) {
        return 0;  // Клиент уже удален
    }
    
    //  Проверяем, не отключился ли клиент
    if (c->timer_id < 0) {
        return 0;  // Таймер уже отменен
    }
    
    // Проверяем, не истекло ли время ожидания клиента
    int64_t now_ms = get_monotonic_time_us() / 1000;
    int64_t last_active_ms = c->last_activity_us / 1000;
    
    if (now_ms - last_active_ms >= CLIENT_TIMEOUT_MS) {
        // Время ожидания истекло, закрываем соединение
        
        // Удаляем файловые события
        qDeleteFileEvent(eventLoop, c->fd, Q_READABLE | Q_WRITABLE);
        
        // Закрываем сокет
        close(c->fd);
        
        // Освобождаем память клиента
        qfree(c);
        
        return 1; // Клиент был удален
    }
    
    return 0; // Клиент не был удален
}