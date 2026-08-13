#ifndef CORE_H
#define CORE_H

#include "utils.h"
#include "../server/server.h"

// макрос для паники
#define panic(...) panic(__FILE__,__LINE__, __VA_ARGS__)

extern Qserver *g_server;




#endif // CORE_H
