//
// Created by louismmassin on 10/9/24.
//
#include "alloc_3is.h"

long magicNumber = 0x0123456789ABCDEFL;
HEADER * freeBlockListHead = NULL;

void * malloc_3is(size_t dataSize) {
    HEADER * headerPtr = findBlockOfExactSize(dataSize);
    if(headerPtr!=NULL) {
        headerPtr->ptr_next = NULL;
        void * dataAddress = (void *)(headerPtr + 1);
        return dataAddress;
    }
    headerPtr = findBigBlock(dataSize);
    if(headerPtr != NULL) {
        sliceBigBlock(headerPtr, dataSize);
        headerPtr->ptr_next = NULL;
        void * dataAddress = (void *)(headerPtr + 1);
        return dataAddress;
    }

    if(dataSize > ALLOCMINSIZE) {
        headerPtr = sbrk(sizeof(HEADER)+ dataSize + sizeof(long));
        headerPtr->block_size = dataSize;
        headerPtr->magic_number = random();
        headerPtr->ptr_next = NULL;

        void * blockAddress = (void*)(headerPtr + 1);

        long* longAddress = (blockAddress + dataSize);
        *longAddress = headerPtr->magic_number;
        return blockAddress;
    }
    else {
        headerPtr = sbrk(sizeof(HEADER)+ ALLOCMINSIZE + sizeof(long));
        headerPtr->block_size = ALLOCMINSIZE;
        headerPtr->magic_number = random();
        headerPtr->ptr_next = NULL;

        void * blockAddress = (void*)(headerPtr + 1);

        long* longAddress = (blockAddress + ALLOCMINSIZE);
        *longAddress = headerPtr->magic_number;
        sliceBigBlock(headerPtr, dataSize);
        return blockAddress;
    }
}

void free_3is(void * address) {
    if(freeBlockListHead == NULL) {
        //cas où c'est le premier bloc libéré
        HEADER * headerPtr = (HEADER*)address - 1;
        freeBlockListHead = headerPtr;
    }
    else {
        HEADER* headerPtr = (HEADER*)address - 1;
        HEADER* previousHeaderPtr = findPreviousInFreeList(headerPtr);
        if(previousHeaderPtr == NULL) {
            //cas où le nouveau bloc libéré doit être placé en premier
            headerPtr->ptr_next = freeBlockListHead;
            freeBlockListHead = headerPtr;
            mergeIfAdjacent(freeBlockListHead, freeBlockListHead->ptr_next);
        }
        else if(previousHeaderPtr->ptr_next == NULL) {
            //cas où le nouveau bloc libéré est à la fin
            previousHeaderPtr->ptr_next = headerPtr;
            mergeIfAdjacent(previousHeaderPtr, headerPtr);
        }
        else {
            //cas général
            HEADER* nextHeaderPtr = previousHeaderPtr->ptr_next;
            previousHeaderPtr->ptr_next = headerPtr;
            headerPtr->ptr_next = nextHeaderPtr;
            mergeIfAdjacent(headerPtr,nextHeaderPtr);
            mergeIfAdjacent(previousHeaderPtr, headerPtr);
        }
    }
}

int check_memory(void * address) {
    HEADER* headerAddress = (HEADER *)address - 1;
    long leftWatchdog = headerAddress->magic_number;
    size_t size = headerAddress->block_size;
    void * right_address = address + size;
    long rightWatchdog = *(long*)(right_address);
    if (leftWatchdog == rightWatchdog) {
        return 0;
    }
    return 1;
}

void allocTest() {
    printf("size of an int: %lu\n", sizeof(int));
    printf("size of a long: %lu\n", sizeof(long));
    printf("size of a header: %lu\n", sizeof(HEADER));
    int arraySize = 9;
    int* testArray = malloc_3is(arraySize * sizeof(int));
    printf("address testArray : %p \n", testArray);
    for(int i = 0; i < arraySize; i++) {
        *(testArray + i) = i;
    }
    printf("test number: %d\n", *(testArray+ 4));
    int check = check_memory(testArray);
    printf("check: %d\n", check);
    free_3is(testArray);
    void* testAddress = malloc_3is(42);

    int arraySize2 = 8;
    int * testArray2 = malloc_3is(arraySize2 * sizeof(int));
    free_3is(testArray2);

    int arraySize3 = 100;
    int * testArray3 = malloc_3is(arraySize3 * sizeof(int));
    //HEADER* headerPtr3 = (HEADER*)testArray3 - 1;
    free_3is(testArray3);

    int arraySize4 = 5;
    int * testArray4 = malloc_3is(arraySize4 * sizeof(int));
    //HEADER* headerPtr4 = (HEADER*)testArray4 - 1;
    free_3is(testArray4);

    int* testArray5 = malloc_3is(arraySize * sizeof(int));
    int * testArray6 = malloc_3is(2 * sizeof(int));

}

HEADER * findBlockOfExactSize(size_t dataSize) {
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

HEADER * findBigBlock(size_t dataSize) {
    if(freeBlockListHead == NULL) {
        return NULL;
    }
    size_t requiredSize = dataSize + sizeof(HEADER) + sizeof(long);
    if(freeBlockListHead->block_size > requiredSize) {
        HEADER * selectedBlock = freeBlockListHead;
        freeBlockListHead = freeBlockListHead->ptr_next;
        return selectedBlock;
    }
    HEADER * currentBlock = freeBlockListHead;
    while(1) {
        if(currentBlock->ptr_next == NULL) {
            return NULL;
        }
        if(currentBlock->ptr_next->block_size > requiredSize) {
            //taking the block out of the list
            HEADER * selectedBlock = currentBlock->ptr_next;
            currentBlock->ptr_next = selectedBlock->ptr_next;
            return selectedBlock;
        }
        currentBlock = currentBlock->ptr_next;
    }
}

void sliceBigBlock(HEADER * bigBlock, size_t dataSize) {
    const size_t oldSize = bigBlock->block_size;

    //modifying the size of the block
    bigBlock->block_size = dataSize;
    void* dataPtr = (void*)(bigBlock + 1);
    long* watchdogPtr = (long*)(dataPtr + dataSize);
    *watchdogPtr = bigBlock->magic_number;

    //creating a free block from the remaining space
    HEADER* newHeaderPtr = (HEADER*)(watchdogPtr + 1);
    newHeaderPtr->ptr_next = NULL;
    newHeaderPtr->block_size = oldSize - dataSize - sizeof(long) - sizeof(HEADER);
    newHeaderPtr->magic_number = random();
    long* newWatchdogPtr = (long*)((void*)(newHeaderPtr + 1) + newHeaderPtr->block_size);
    *newWatchdogPtr = newHeaderPtr->magic_number;
    void* newDataPtr = (void*)(newHeaderPtr+1);
    free_3is(newDataPtr);
}

HEADER * findPreviousInFreeList(HEADER* headerPtr) {
    if(freeBlockListHead >= headerPtr) {
        return NULL;
    }
    else {
        HEADER * freeBlockList = freeBlockListHead;
        while(1) {
            if(freeBlockList->ptr_next == NULL) {
                return freeBlockList;
            }
            else if (freeBlockList->ptr_next >= headerPtr){
                return freeBlockList;
            }
            else {
                freeBlockList = freeBlockList->ptr_next;
            }
        }
    }
}

void mergeIfAdjacent(HEADER * leftBlockPtr, HEADER* rightBlockPtr) {
    if ((void*)leftBlockPtr == (void*)rightBlockPtr - sizeof(long) - leftBlockPtr->block_size - sizeof(HEADER)) {
        long* rightWatchdogPtr =  (long*)((void*)(rightBlockPtr + 1) + rightBlockPtr->block_size);
        *rightWatchdogPtr = leftBlockPtr->magic_number;
        leftBlockPtr->ptr_next = rightBlockPtr->ptr_next;
        leftBlockPtr->block_size += sizeof(long) + rightBlockPtr->block_size + sizeof(HEADER);

    }
}