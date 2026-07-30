#ifndef QORALOOP_H
#define QORALOOP_H

#define Q_NONE 0
#define Q_READABLE 0x0001
#define Q_WRITABLE 0x0002
#define Q_ERROR -1 
#define Q_OK 0
#define Q_TIMER_DEL 1
#define TIMER_ACTIVE 0
#define EMPTY_TIMERS -1
#define CLIENT_TIMEOUT_MS 30000 // 30 секунд

#include <stdint.h>

typedef struct qEventLoop qEventLoop;
typedef void (*qFileProc)(qEventLoop *eventLoop, int fd, void *clientData, int mask);
typedef int (*qTimerProc)(qEventLoop* loop, int fd, void* clientData, int mask);

typedef struct qFileEvent {
    int mask;
    qFileProc rfileProc;
    qFileProc wfileProc;
    void *clientData;
} qFileEvent;

typedef struct qFiredEvent {
    int fd;
    int mask;
} qFiredEvent;

typedef struct qTimerEvent {
    long id;
    int deleted;
    qTimerProc proc; // Теперь правильный тип - qTimerProc
    int64_t when_ms;
    void* Client;
} qTimerEvent;

typedef struct qEventLoop {
    int maxfd;
    int size;
    void *apidata;
    int stop;
    int nevents;
    qFileEvent* events;
    qFiredEvent* fired;
    qTimerEvent** timers;
    int size_timers;
    int count_timers;
} qEventLoop;

// Функции таймеров
int add_timer(qEventLoop* loop, int64_t when_ms, qTimerProc proc, void* data);
void cancel_timer(qEventLoop* loop, int id);
void free_timer(qTimerEvent* timer);
int64_t search_timer(qEventLoop* loop);
void process_timers(qEventLoop* eventLoop);

// Основные функции event loop
qEventLoop *qCreateLoop(int size);
void qDeleteLoop(qEventLoop *loop);
int qCreateFileEvent(qEventLoop *loop, int fd, int mask, qFileProc proc, void *clientData);
void qDeleteFileEvent(qEventLoop *loop, int fd, int delmask);
int qProcessEvents(qEventLoop *loop);
void qMain(qEventLoop *loop);

// Вспомогательные функции
int64_t get_monotonic_time_us(void);

#endif // QORALOOP_H