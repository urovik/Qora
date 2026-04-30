#include "memory.h"
#include "core.h"

void* qfree(void* ptr){
    free(ptr);
}


void* qmalloc(size_t size){
    void* ptr = malloc(size);
    if(ptr == NULL){
        panic("error: failed memory alloc");
    }
}

void* qrealloc(void* ptr, size_t size){
    void* new_ptr = realloc(ptr, size);
    if(new_ptr == NULL){
        panic("error: failed memory alloc");
    }
}

void* qcalloc(size_t n, size_t size){
    void* ptr = calloc(n, size);
    if(ptr == NULL){
        panic("error: failed memory alloc");
    }

}