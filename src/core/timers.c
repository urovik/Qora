#include "timers.h"
#include <errno.h>

int add_timer(qEventLoop* loop, int64_t when_ms, qTimerProc proc, void* data) {
    if (!loop) return -1;
    
    // Проверяем, есть ли свободное место
    if (loop->count_timers >= loop->size_timers) {
        int new_size = loop->size_timers * 2;
        if (new_size < 16) new_size = 16;
        
        qTimerEvent** new_timers = qrealloc(loop->timers, sizeof(qTimerEvent*) * new_size);
        if (!new_timers) return -1;
        
        for (int i = loop->size_timers; i < new_size; i++) {
            new_timers[i] = NULL;
        }
        
        loop->timers = new_timers;
        loop->size_timers = new_size;
    }
    
    qTimerEvent* new_timer = qmalloc(sizeof(qTimerEvent));
    if (!new_timer) return -1;
    
    // Находим свободный индекс
    int free_id = -1;
    for (int i = 0; i < loop->size_timers; i++) {
        if (loop->timers[i] == NULL) {
            free_id = i;
            break;
        }
    }
    
    if (free_id == -1) {
        qfree(new_timer);
        return -1;
    }
    
    // ВАЖНО: вычисляем абсолютное время срабатывания
    int64_t now_ms = get_monotonic_time_us() / 1000;
    int64_t absolute_when = now_ms + when_ms; // when_ms - это задержка в миллисекундах
    
    printf("add_timer: now=%ld, delay=%ld, absolute_when=%ld\n", 
           now_ms, when_ms, absolute_when);
    
    // Инициализируем таймер
    new_timer->id = free_id;
    new_timer->deleted = TIMER_ACTIVE;
    new_timer->proc = proc;
    new_timer->when_ms = absolute_when; // Сохраняем АБСОЛЮТНОЕ время
    new_timer->Client = data;
    
    loop->timers[free_id] = new_timer;
    loop->count_timers++;
    
    return free_id;
}

void cancel_timer(qEventLoop* loop, int id) {
    if (id < 0 || id >= loop->size_timers) return;
    if (loop->timers[id] == NULL) return;
    
    qTimerEvent* timer = loop->timers[id];
    timer->deleted = Q_TIMER_DEL;
    loop->count_timers--;
}

void free_timer(qTimerEvent* timer) {
    if (timer) qfree(timer);
}

int64_t search_timer(qEventLoop* loop) {
    int64_t when_ms = -1;
    if (loop->count_timers == 0) {
        return EMPTY_TIMERS;
    }
    
    for (int i = 0; i < loop->size_timers; i++) {
        if (loop->timers[i] != NULL && loop->timers[i]->deleted != Q_TIMER_DEL) {
            if (when_ms == -1) {
                when_ms = loop->timers[i]->when_ms;
            }
            if (loop->timers[i]->when_ms < when_ms) {
                when_ms = loop->timers[i]->when_ms;
            }
        }
    }
    return when_ms; // Возвращаем в миллисекундах
}