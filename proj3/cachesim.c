#include <stdio.h>
#include <stdlib.h>
#include "cachesim.h"

struct Node {
    int value;
    struct Node *next;
};

typedef struct list {
    struct Node *head;
    struct Node *tail;
} List;

typedef struct cache {
    int type;
    int accessCount;
    int missCount;
    int accessTime;
    int blocksize;
    int cachesize;
    int assocCacheSize;
    int* dm;
    List* assocCache;
} Cache;

int integer_divide(int divisor1, int divisor2) {
    while(divisor2 > 1) {
        //printf("%d, %d\n", divisor1, divisor2);
        divisor1 = divisor1 >> 1;
        divisor2 = divisor2 >> 1;
    }
    return divisor1;
}

int be_mod(int address, int cachesize, int blocksize){
   int modulus = (cachesize - 1) & address;
   return integer_divide(modulus, blocksize);
}


int bitwise_equals(int addee1, int addee2){
    return !(addee1 - addee2);
}

// handles increasing miss count (if they fail to find), increase access time upon success or failure, handle update
int checkVC(Cache *cashay, int address){
    //instantiates the return value
    int retval = 0;

    int count = 0;

    //creates space on the heap for the new node that we will
    //put onto the victim cache if we need to
    struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));

    //intializes the next pointer of the new node to NULL
    newNode->next = NULL;

    //for our loop we initialize a prev and next pointer to traverse
    //the linked list
    struct Node *prev = NULL;
    struct Node *next = cashay->assocCache->head;

    //gives us the index in the dm cache we wish to compare against
    //our address
    int dm_index = be_mod(address, cashay->cachesize, cashay->blocksize);

    //loops over the linked list to see if we can find our address in the
    //victim cache
    while(next != NULL){
        count++;
        if(bitwise_equals(next->value,integer_divide(address, cashay->blocksize))){
            newNode->value = cashay->dm[dm_index];
            cashay->dm[dm_index] = integer_divide(address, cashay->blocksize);
            cashay->accessTime+=6;
            retval = 1;
            if (prev != NULL) {
                prev->next = next->next;
                if (cashay->assocCacheSize == count) {
                    cashay->assocCache->tail = prev;
                }
            }
            else {
                cashay->assocCache->head = next->next;
                if (bitwise_equals(cashay->assocCacheSize,1)) {
                    cashay->assocCache->tail = cashay->assocCache->head;
                }
            }
            free(next);
            cashay->assocCacheSize--;
            break;
        }
        prev = next;
        next = next->next;
    }

    //if our address is not in the victim cache, we increase miss,
    //lose 100 cycles of time, we add the address block to the dm cache,
    //and we make the newNode value be the old dm cache address
    if(bitwise_equals(retval,0)){
        cashay->missCount++;
        cashay->accessTime+=100;
        newNode->value = cashay->dm[dm_index];
        cashay->dm[dm_index] = integer_divide(address, cashay->blocksize);
    }

    //if it is a compulsory miss, we do not add anything to the
    //victim cache
    if (bitwise_equals(newNode->value,-1)) {
        free(newNode);
        return retval;
    }

    //if we missed, we must take the LRU item (the head) off
    //of our victim cache if our cache is full
    if(bitwise_equals(cashay->assocCacheSize, 6)) {
        struct Node * tmp = cashay->assocCache->head;
        cashay->assocCache->head = cashay->assocCache->head->next;
        cashay->assocCacheSize--;
        free(tmp);
    }

    //if we do not have anything on our cache, we automatically
    //point the head to the new node and increase the cache size
    //otherwise, we just add it to the end of the tail
    if (cashay->assocCache->head == NULL) {
        cashay->assocCache->head = newNode;
        cashay->assocCacheSize++;
    }
    else {
        cashay->assocCache->tail->next = newNode;
        cashay->assocCacheSize++;
    }
    //we are updating the tail pointer here
    cashay->assocCache->tail = newNode;

    //return whether or not we hit or miss
    return retval;
}

int checkSP(Cache *cashay, int address){
    //instantiates the return value
    int retval = 0;

    int count = 0;
    //creates space on the heap for the new node that we will
    //put onto the victim cache if we need to
    struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));

    //intializes the next pointer of the new node to NULL
    newNode->next = NULL;
    newNode->value = integer_divide(address, cashay->blocksize) + 1;

    //for our loop we initialize a prev and next pointer to traverse
    //the linked list
    struct Node *prev = NULL;
    struct Node *next = cashay->assocCache->head;

    //gives us the index in the dm cache we wish to compare against
    //our address
    int dm_index = be_mod(address, cashay->cachesize, cashay->blocksize);

    // always puts the new value in the dm cache
    cashay->dm[dm_index] = integer_divide(address, cashay->blocksize);


    //loops over the linked list to see if we can find our address in the
    //victim cache
    while(next != NULL){
        count++;
        if(bitwise_equals(next->value,integer_divide(address, cashay->blocksize))){
            cashay->accessTime+=6;
            retval = 1;
            if (prev != NULL) {
                prev->next = next->next;
                if (bitwise_equals(cashay->assocCacheSize, count)) {
                    cashay->assocCache->tail = prev;
                }
            }
            else {
                cashay->assocCache->head = next->next;
                if (bitwise_equals(cashay->assocCacheSize, 1)) {
                    cashay->assocCache->tail = cashay->assocCache->head;
                }
            }
            free(next);
            cashay->assocCacheSize--;
            break;
        }
        prev = next;
        next = next->next;
    }
    if(bitwise_equals(retval, 0)){
        cashay->missCount++;
        cashay->accessTime+=100;
    }

    if(bitwise_equals(cashay->assocCacheSize, 6)) {
        struct Node * tmp = cashay->assocCache->head;
        cashay->assocCache->head = cashay->assocCache->head->next;
        cashay->assocCacheSize--;
        free(tmp);
    }

    if (cashay->assocCache->head == NULL) {
        cashay->assocCache->head = newNode;
        cashay->assocCacheSize++;
    }
    else {
        cashay->assocCache->tail->next = newNode;
        cashay->assocCacheSize++;
    }

    cashay->assocCache->tail = newNode;
    return retval;
}

void *createAndInitialize(int blocksize, int cachesize, int type){

    Cache * cashay = (Cache *)malloc(sizeof(Cache));
    cashay->dm = (int *)malloc(integer_divide(cachesize*sizeof(int),blocksize));

    int i;
    for (i = 0; i < integer_divide(cachesize, blocksize); i++){
        cashay->dm[i] = -1;
    }

    cashay->type = type;
    cashay->blocksize = blocksize;
    cashay->cachesize = cachesize;

    cashay->accessCount = 0;
    cashay->missCount = 0;
    cashay->accessTime = 0;

    cashay->assocCache = (List *)malloc(sizeof(List));
    cashay->assocCache->head = NULL;
    cashay->assocCache->tail = NULL;
    cashay->assocCacheSize = 0;

    return (void *) cashay;


}
// You have a struct that contains all of the information for one cache.
// In this function, you create the cache and initialize it,
// returning a pointer to the struct. Because you are determining the struct,
// you return a void * to our main. Type 0 is a direct-mapped cache.
// Type 1 is a direct-mapped cache with a victim cache. Type 2 is a direct-mapped cache with a stream prefetcher.


int accessCache(void *cache, int address){
    Cache *cashay = (Cache*) cache; // now cashay... away.
    cashay->accessCount++;
    int dm_index = be_mod(address, cashay->cachesize, cashay->blocksize);
    if(bitwise_equals(cashay->dm[dm_index], integer_divide(address, cashay->blocksize))) {
        cashay->accessTime++;
        return 1;
    }
    switch(cashay->type){
        case 0:
            cashay->missCount++;
            cashay->accessTime+=100;
            cashay->dm[dm_index] = integer_divide(address, cashay->blocksize);
            return 0;
        case 1:
            return checkVC(cashay, address);
        case 2:
            return checkSP(cashay, address);
    }
}

int missesSoFar(void *cache){
    Cache *cashay = (Cache*) cache;
    //return cashay->blocksize;
    return cashay->missCount;
}

int accessesSoFar(void *cache){
    Cache *cashay = (Cache*) cache;
    //return cashay->cachesize;
    return cashay->accessCount;
}


int totalAccessTime(void *cache){
    Cache* cashay = (Cache*) cache;
    return cashay->accessTime;
}
