#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <stdio.h>

void* qmalloc(size_t size);
void* qrealloc(void* ptr, size_t size);
void* qcalloc(size_t n, size_t size);
void* qfree(void* ptr);




#endif // MEMORY_H
