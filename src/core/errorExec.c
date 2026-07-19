#include "errorExec.h"



/*
 * Copyright (c) 2026, urovik
 * Licensed under the BSD-3-Clause license. See LICENSE file in the root directory.
 */


Error err;

inline void set_err(Error* err, int l, int c, const char* m) {
    err->line = l; err->column = c;
    snprintf(err->message, sizeof(err->message), "%s", m);
}