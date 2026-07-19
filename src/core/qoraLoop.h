#ifndef QORALOOP_H
#define QORALOOP_H

#define Q_NONE 0
#define Q_READABLE 0x0001 // 1 бит готов для чтения (fd)
#define Q_WRITABLE 0x0002 // 2 бит готов для записи (fd)
#define Q_ERROR -1 
#define Q_OK 0
#define Q_TIMER_DEL 1
#define TIMER_ACTIVE 0
#define EMPTY_TIMERS -1


#include <stdint.h>


typedef struct qEventLoop qEventLoop;
// Тип функции-обработчика (указатель на функцию)
typedef void (*qFileProc)(qEventLoop *eventLoop, int fd, void *clientData, int mask);

typedef struct qFileEvent {
    int mask; /* one of AE_(READABLE|WRITABLE|BARRIER) */
    qFileProc rfileProc;
    qFileProc wfileProc;
    void *clientData;

} qFileEvent;


typedef struct qFiredEvent {
    int fd;
    int mask;
} qFiredEvent;

typedef struct qTimerEvent
{
    long id;
    int deleted; // флаг для таймера
    qFileProc proc;
    int64_t when_ms;
    void* Client;

} qTimerEvent;


typedef struct qEventLoop{

    int maxfd; // самый наивысший fd
    int size; // количество fd которые может отслеживать loop
    void *apidata; // делается void* чтобы в дальнейшем добавить совместимость с kqueue и select
    int stop; // флаг для корректно завершения цикла если stop == 1
    int nevents; // количество зарегистрированных fd
    qFileEvent* events;
    qFiredEvent* fired;
    qTimerEvent** timers; // пока что просто массив, нужна структура по лучше, либо мин-хип, лиюо попробовать в dict обернуть чтобы o(1)
    int size_timers;
    int count_timers; // счетчик активных таймеров

} qEventLoop;


void process_timers(qEventLoop* eventLoop);

qEventLoop *qCreateLoop(int size);
void qDeleteLoop(qEventLoop *loop);
int qCreateFileEvent(qEventLoop *loop, int fd, int mask, qFileProc proc, void *clientData);
void qDeleteFileEvent(qEventLoop *loop, int fd, int delmask);
int qProcessEvents(qEventLoop *loop);
void qMain(qEventLoop *loop);



#endif // QORALOOP_H
