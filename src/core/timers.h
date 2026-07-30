#ifndef TIMERS_H
#define TIMERS_H

#include "qoraLoop.h"
#include "memory.h"

// Прототипы функций таймеров
int add_timer(qEventLoop* loop, int64_t when_ms, qTimerProc proc, void* data);
void cancel_timer(qEventLoop* loop, int id);
void free_timer(qTimerEvent* timer);
int64_t search_timer(qEventLoop* loop);

#endif // TIMERS_H