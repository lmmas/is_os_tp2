//
// Created by louismmassin on 10/9/24.
//
#include "alloc_3is.h"

typedef struct HEADER_TAG {
    struct HEADER_TAG * ptr_next;
    size_t block_size;
    long magic_number;
} HEADER;

long magicNumber = 0x0123456789ABCDEFL;
HEADER * freeBlockListHead = NULL;
HEADER * freeBlockList;

void * malloc_3is(size_t dataSize) {

    void * oldAddress = sbrk(sizeof(HEADER)+ dataSize + sizeof(long));

    HEADER* headerAddress = oldAddress;
    headerAddress->block_size = dataSize;
    headerAddress->magic_number = magicNumber;
    headerAddress->ptr_next = NULL;

    void * blockAddress = oldAddress + sizeof(HEADER);

    long* longAddress = (blockAddress + dataSize);
    *longAddress = magicNumber;
    return blockAddress;

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

int check_memory(void * address) {
    HEADER* headerAddress = address;
    headerAddress--;
    long leftWatchdog = headerAddress->magic_number;
    size_t size = headerAddress->block_size;
    void * right_address = address + size;
    long rightWatchdog = *(long*)(right_address);
    if (leftWatchdog == magicNumber && rightWatchdog == magicNumber) {
        return 0;
    } else {
        return 1;
    }
}

void allocTest() {
    int arraySize = 8;
    int* testArray = malloc_3is(arraySize * sizeof(int));
    printf("address testArray : %p \n", testArray);
    for(int i = 0; i < arraySize; i++) {
        *(testArray + i) = i;
    }
    printf("test number: %d\n", *(testArray+ 4));
    free_3is(testArray);
    int arraySize2 = 9;
    int * testArray2 = malloc_3is(arraySize2 * sizeof(int));
    int check = check_memory(testArray);
    printf("check: %d\n", check);
    free_3is(testArray2);
}