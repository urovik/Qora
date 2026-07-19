



// надо бы допились qrealloc если нет свободных мест в массиве
void add_timer(qEventLoop* loop, int64_t when, qFileProc proc, void* data){
    
    qTimerEvent* new_timer = qmalloc(sizeof(qTimerEvent));
    new_timer->Client = (Client*)data;
    new_timer->deleted = TIMER_ACTIVE;
    new_timer->proc = proc;
    new_timer->when_ms = when;

    // нахождение свободного индекса в массиве таймеров
    for(int n = 0; n < loop->size_timers; n++){
        if(loop->timers[n] == NULL){
            new_timer->id = n; // индекс где в массиве находится этот таймер, пока оставим так, нужно реализовать потом быстрее
            loop->timers[n] = new_timer;
            loop->count_timers++;
            return; // выходим из цикла после добавление таймера, чтобы ничего не перезаписать
        }
        // реализовать блок else в случае если в массиве таймеров нет свободного места
    }
}
void cancel_timer(qEventLoop* loop, int id){
    if (id < 0 || id >= loop->size_timers) return;
    if (loop->timers[id] == NULL) return;
    // если индекс таймера равен id таймера 
    qTimerEvent* timer = loop->timers[id];
    timer->deleted = Q_TIMER_DEL;
}

void free_timer(qTimerEvent* timer) {
    if (timer) qfree(timer);
}

int64_t search_timer(qEventLoop* loop){
    int when_ms = -1;
    if(loop->count_timers == 0){
        return EMPTY_TIMERS; // -1 для того чтобы если таймеров нет, вернуть в epoll -1 
    }
    for (int i = 0; i < loop->size_timers; i++){
        if(loop->timers[i] != NULL && loop->timers[i]->deleted != Q_TIMER_DEL){
        if(when_ms == -1){
            when_ms = loop->timers[i]->when_ms;
        }
        if(loop->timers[i]->when_ms < when_ms){
            when_ms = loop->timers[i]->when_ms;
        } 
        }
    }
    return when_ms / 1000; // делим на 1000 чтобы вернуть миллисенкуды
}

