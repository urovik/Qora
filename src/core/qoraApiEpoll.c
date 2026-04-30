#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>

#include <core/core.h>
#include <core/memory.h>


typedef struct {
    int epfd;
    struct epoll_event *events;
} qApiState;

static int qApiCreate(qEventLoop* qEventLoop) {
    qApiState *state = qmalloc(sizeof(qApiState));
    if (!state) return -1;
    state->events = qmalloc(sizeof(struct epoll_event) * qEventLoop->size);
    if (!state->events) {
        qfree(state);
        return -1;
    }
    state->epfd = epoll_create1(0);
    if (state->epfd == -1) {
        qfree(state->events);
        qfree(state);
        return -1;
    }
    qEventLoop->apidata = state;
    return 0;  // добавлено
}


static int qApiResize(qEventLoop* qEventLoop){
    qApiState *state = qEventLoop->apidata;

    state->events = qrealloc(state->events, sizeof(struct epoll_event) * qEventLoop->size);

    return 0;
}

static void qApiFree(qEventLoop* qEventLoop){
    qApiState *state = qEventLoop->apidata;

    close(state->epfd);
    qfree(state->events);
    qfree(state);
    
}


static int qApiAddEvent(qEventLoop* qEventLoop,int fd, int mask){
    qApiState *state = qEventLoop->apidata;
    int op;
    struct epoll_event ee = {0};

    if(qEventLoop->events[fd].mask == Q_NONE){
        op = EPOLL_CTL_ADD;
    } else{
        op = EPOLL_CTL_MOD;
    }
    ee.events = 0;
    mask |= qEventLoop->events[fd].mask; // делаем слияние масок чтобы не потерять уже имеющиюся
    
    if(mask & Q_WRITABLE) ee.events |= EPOLLOUT;
    if(mask & Q_READABLE) ee.events |= EPOLLIN;

    ee.data.fd = fd;
    // добавляем fd в ядро для отслеживания, перменная ee после epoll_ctl не нужна так как ядро получило настройку 
    if(epoll_ctl(state->epfd,op,fd,&ee) == -1) return -1;

    return 0;
}

static void qApiDelEvent(qEventLoop* qEventLoop, int fd, int delmask) {
    qApiState *state = qEventLoop->apidata;
    struct epoll_event ee = {0};
    int mask = qEventLoop->events[fd].mask & (~delmask);
    // исправлено: == на =
    ee.events = 0;
    if (mask & Q_READABLE) ee.events |= EPOLLIN;
    if (mask & Q_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (mask != Q_NONE) {
        epoll_ctl(state->epfd, EPOLL_CTL_MOD, fd, &ee);
    } else {
        epoll_ctl(state->epfd, EPOLL_CTL_DEL, fd, &ee);
    }
}


static int qApiEpoll(qEventLoop* qEventLoop) {
    qApiState* state = qEventLoop->apidata;
    int nevents = 0;  // инициализация
    int queue_fd = epoll_wait(state->epfd, state->events, qEventLoop->size, -1);
    if (queue_fd > 0) {
        nevents = queue_fd;
        for (int i = 0; i < nevents; i++) {   // исправлено: i=0
            int mask = 0;
            struct epoll_event* e = state->events + i;
            if (e->events & EPOLLIN) mask |= Q_READABLE;
            if (e->events & EPOLLOUT) mask |= Q_WRITABLE;
            if (e->events & (EPOLLHUP | EPOLLERR)) mask |= Q_READABLE | Q_WRITABLE;
            qEventLoop->fired[i].fd = e->data.fd;
            qEventLoop->fired[i].mask = mask;
        }
    } else if (queue_fd == -1 && errno != EINTR) {
        panic("aeApiPoll: epoll_wait, %s", strerror(errno));
    }
    return nevents;
}