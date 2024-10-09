//
// Created by louismmassin on 10/9/24.
//

#ifndef ALLOC_3IS_H
#define ALLOC_3IS_H
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
void * malloc_3is(size_t dataSize);
void allocTest();
void free_3is(void * address);
int check_memory(void * address);
#endif //ALLOC_3IS_H
