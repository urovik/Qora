#include "qoraLoop.h"
#include "memory.h"

#include "qoraApiEpoll.c"

#include <errno.h>


qEventLoop *qCreateLoop(int size){
    // выделяем память под event loop
    qEventLoop* evLoop = qmalloc(sizeof(qEventLoop));
    evLoop->size = size;
    evLoop->events = qmalloc(sizeof(qFileEvent)  * size);
    evLoop->fired = qmalloc(sizeof(qFiredEvent) * size);
    evLoop->maxfd = -1;
    evLoop->stop = 0;
    evLoop->nevents = size;
    if(qApiCreate(evLoop) == -1) goto err;

    for(int i = 0; i < evLoop->nevents; i++){
        evLoop->events[i].mask = Q_NONE;
    }
    return evLoop;

err:
    if(evLoop) {
        qfree(evLoop->events);
        qfree(evLoop->fired);
        qfree(evLoop);
    }
    return NULL;
}

void qDeleteLoop(qEventLoop *loop){
    qApiFree(loop);
    qfree(loop->fired);
    qfree(loop->events);
    qfree(loop);
}


int qCreateFileEvent(qEventLoop *eventLoop, int fd, int mask, qFileProc proc, void *clientData){
    if(fd > eventLoop->size){
        errno = ERANGE;
        return Q_ERROR;
    }

    /* Resize the events and fired arrays if the file
     * descriptor exceeds the current number of events. */
    if (fd >= eventLoop->nevents) {
        int newnevents = eventLoop->nevents;
        newnevents = (newnevents * 2 > fd + 1) ? newnevents * 2 : fd + 1;
        newnevents = (newnevents > eventLoop->size) ? eventLoop->size : newnevents;
        eventLoop->events = qrealloc(eventLoop->events, sizeof(qFileEvent) * newnevents);
        eventLoop->fired = qrealloc(eventLoop->fired, sizeof(qFiredEvent) * newnevents);

        for(int i = eventLoop->nevents; i < newnevents; i++){
            eventLoop->events[i].mask = Q_NONE;
        }
        eventLoop->nevents = newnevents;
    }
    qFileEvent* fe = &eventLoop->events[fd];
    if(qApiAddEvent(eventLoop, fd, mask) == -1){
        return Q_ERROR;
    }
    fe->mask |= mask;
    if(mask & Q_READABLE) fe->rfileProc = proc;
    if(mask & Q_WRITABLE) fe->wfileProc = proc;

    fe->clientData = clientData;
    if(fd > eventLoop->maxfd){
        eventLoop->maxfd = fd;
    }
    return Q_OK;
}
void qDeleteFileEvent(qEventLoop *eventLoop, int fd, int delmask){

    if(fd >= eventLoop->size) return;

    qFileEvent* fe = &eventLoop->events[fd];

    qApiDelEvent(eventLoop, fd, delmask);
    fe->mask = fe->mask & (~delmask);

    // чистим ev от fd с пустыми масками
    if (fd == eventLoop->maxfd && fe->mask == Q_NONE) {
        /* Update the max fd */
        int j;

        for (j = eventLoop->maxfd-1; j >= 0; j--)
            if (eventLoop->events[j].mask != Q_NONE) break;
        eventLoop->maxfd = j;
    }

}

int qProcessEvents(qEventLoop *eventLoop){
    int processed = 0, numevents;

    // вернет количество fd которые нуждаются в обработке
    numevents = qApiEpoll(eventLoop);

    for(int i = 0; i < numevents; i++){
        int fd = eventLoop->fired[i].fd;
        qFileEvent *fe = &eventLoop->events[fd];
        int mask = eventLoop->fired[i].mask;
        int fired = 0;

        if(mask & Q_READABLE){
            fe->rfileProc(eventLoop, fd, fe->clientData, mask);
            fired++;
            fe = &eventLoop->events[fd]; // защита от потенциального realloc
        }

        if(mask & Q_WRITABLE){
            fe->wfileProc(eventLoop, fd, fe->clientData, mask);
            fired++;

        }
        processed++;
    }
    return processed;

}

void qMain(qEventLoop *evLoop){
    evLoop->stop = 0;
    while(!evLoop->stop){
        qProcessEvents(evLoop);

    }
}