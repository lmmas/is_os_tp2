//
// Created by louismmassin on 10/9/24.
//
#include "alloc_3is.h"

long magicNumber = 0x0123456789ABCDEFL;
HEADER * freeBlockListHead = NULL;

void * malloc_3is(size_t dataSize) {
    HEADER * emptyBlockAddress = findBlock(dataSize);
    if(emptyBlockAddress!=NULL) {
        emptyBlockAddress++;
        return (void *)emptyBlockAddress;
    }
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
    HEADER* headerPtr = address;
    headerPtr--;
    if(freeBlockListHead == NULL) {
        freeBlockListHead = headerPtr;
    }
    else if(freeBlockListHead->block_size >= headerPtr->block_size) {
        HEADER * tempPtr = freeBlockListHead;
        freeBlockListHead = headerPtr;
        freeBlockListHead->ptr_next = tempPtr;
    }
    else {
        HEADER * freeBlockList = freeBlockListHead;
        int loopEnd = 0;
        while(loopEnd == 0) {
            if(freeBlockList->ptr_next == NULL) {
                freeBlockList->ptr_next = headerPtr;
                loopEnd = 1;
            }
            else if (freeBlockList->ptr_next->block_size >= headerPtr->block_size){
                HEADER* tempPtr = freeBlockList->ptr_next;
                freeBlockList->ptr_next = headerPtr;
                headerPtr->ptr_next = tempPtr;
                loopEnd = 1;
            }
            else {
                freeBlockList = freeBlockList->ptr_next;
            }
        }
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
    }
    return 1;
}

void allocTest() {
    int arraySize = 9;
    int* testArray = malloc_3is(arraySize * sizeof(int));
    printf("address testArray : %p \n", testArray);
    for(int i = 0; i < arraySize; i++) {
        *(testArray + i) = i;
    }
    printf("test number: %d\n", *(testArray+ 4));
    free_3is(testArray);
    int arraySize2 = 8;
    int * testArray2 = malloc_3is(arraySize2 * sizeof(int));
    int check = check_memory(testArray);
    printf("check: %d\n", check);
    free_3is(testArray2);
    int arraySize3 = 10;
    int * testArray3 = malloc_3is(arraySize3 * sizeof(int));
    free_3is(testArray3);
    int arraySize4 = 5;
    int * testArray4 = malloc_3is(arraySize4 * sizeof(int));
    free_3is(testArray4);

    int* testArray5 = malloc_3is(arraySize * sizeof(int));

}

HEADER * findBlock(size_t dataSize) {
    if(freeBlockListHead == NULL) {
        return NULL;
    }
    HEADER * currentBlock = freeBlockListHead;
    while(1) {
        if(currentBlock->ptr_next == NULL) {
            return NULL;
        }
        if(currentBlock->ptr_next->block_size == dataSize) {
            HEADER * selectedBlock = currentBlock->ptr_next;
            currentBlock->ptr_next = selectedBlock->ptr_next;
            return selectedBlock;
        }
        currentBlock = currentBlock->ptr_next;
    }
}
