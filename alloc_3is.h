//
// Created by louismmassin on 10/9/24.
//

#ifndef ALLOC_3IS_H
#define ALLOC_3IS_H
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct HEADER_TAG {
    struct HEADER_TAG * ptr_next;
    size_t block_size;
    long magic_number;
} HEADER;

void * malloc_3is(size_t dataSize);
void allocTest();
void free_3is(void * address);
int check_memory(void * address);
HEADER * findBlockOfExactSize(size_t dataSize);
HEADER * findBigBlock(size_t dataSize);
void sliceBigBlock(HEADER * bigBlock, size_t dataSize);
#endif //ALLOC_3IS_H
