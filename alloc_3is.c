//
// Created by louismmassin on 10/9/24.
//
#include "alloc_3is.h"

typedef struct HEADER_TAG {
    struct HEADER_TAG * ptr_next;
    size_t block_size;
    long magic_number;
} HEADER;

void * malloc_3is(size_t dataSize) {

    long newMagicNumber = 0x0123456789ABCDEFL;

    void * oldAddress = sbrk(sizeof(HEADER)+ dataSize + sizeof(long));

    HEADER* headerAddress = oldAddress;
    HEADER newHeader = *headerAddress;
    newHeader.block_size = dataSize;
    newHeader.magic_number = newMagicNumber;

    void * blockAddress = oldAddress + sizeof(HEADER);

    long* longAddress = (blockAddress + dataSize);
    *longAddress = newMagicNumber;
    return blockAddress;

}

void allocTest() {
    int arraySize = 2;
    int* testArray = malloc_3is(arraySize * sizeof(int));
    printf("address testArray : %p \n", testArray);
    int* testArray2 = malloc(arraySize * sizeof(int));
    printf("address testArray2 : %p \n", testArray2);
    for(int i = 0; i < arraySize; i++) {
        *(testArray + i) = i;
    }
    long magicNumber = *(long*)(((void*) testArray) + arraySize*sizeof(int));
    printf("magic number: %lX\n", magicNumber);
}

void free_3is(void * address) {



}