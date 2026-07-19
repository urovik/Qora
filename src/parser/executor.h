#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "ast.h"
#include "parser.h"
#include "qoraDb.h"


int execute_set_value(QoraDB* qoraDb, Parser* parser);


#endif // EXECUTOR_H