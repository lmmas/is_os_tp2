//
// Created by louismmassin on 10/9/24.
//
#include "alloc_3is.h"

typedef struct HEADER_TAG {
    struct HEADER_TAG * ptr_next;
    size_t block_size;
    long magic_number;
} HEADER;

long newMagicNumber = 0x0123456789ABCDEFL;
HEADER * freeBlockListHead = NULL;
HEADER * freeBlockList;

void * malloc_3is(size_t dataSize) {

    void * oldAddress = sbrk(sizeof(HEADER)+ dataSize + sizeof(long));

    HEADER* headerAddress = oldAddress;
    headerAddress->block_size = dataSize;
    headerAddress->magic_number = newMagicNumber;
    headerAddress->ptr_next = NULL;

    void * blockAddress = oldAddress + sizeof(HEADER);

    long* longAddress = (blockAddress + dataSize);
    *longAddress = newMagicNumber;
    return blockAddress;

}

void allocTest() {
    int arraySize = 8;
    int* testArray = malloc_3is(arraySize * sizeof(int));
    printf("address testArray : %p \n", testArray);
    for(int i = 0; i < arraySize; i++) {
        *(testArray + i) = i;
    }
    printf("test number: %d", *(testArray+ 4));
    free_3is(testArray);
    int arraySize2 = 9;
    int * testArray2 = malloc_3is(arraySize2 * sizeof(int));
    free_3is(testArray2);
}

void free_3is(void * address) {
    HEADER* headerAddress = address;
    headerAddress--;
    if(freeBlockListHead == NULL) {
        freeBlockListHead = headerAddress;
        freeBlockList = freeBlockListHead;
    }
    else {
        freeBlockList->ptr_next = headerAddress;
        freeBlockList = freeBlockList->ptr_next;
    }
}

