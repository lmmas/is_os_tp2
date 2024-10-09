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
    HEADER newHeader;
    newHeader.block_size = dataSize;
    long newMagicNumber = 0x0123456789ABCDEFL;
    newHeader.magic_number = newMagicNumber;
    void * oldAddress = sbrk(0);
    void * newAddress = sbrk(sizeof(HEADER)+ dataSize + sizeof(long));

    HEADER* headerAddress = oldAddress;
    *headerAddress = newHeader;

    void * blockAddress = oldAddress + sizeof(HEADER);

    long* longAddress = (blockAddress + dataSize);
    *longAddress = newMagicNumber;
    printf("%p, %lX\n",longAddress, *longAddress );
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
    //long magicNumber = *(long*)(((void*) testArray) + arraySize*sizeof(int));
    long magicNumber = *(testArray + arraySize);
    printf("%p", testArray + arraySize);
    printf("magic number: %lX\n", magicNumber);
    printf("sizeof int: %ld\n", sizeof(int));
    printf("sizeof long: %ld\n", sizeof(long));
}