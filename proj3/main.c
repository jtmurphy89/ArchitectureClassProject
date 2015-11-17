#include <stdio.h>
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


int main() {

	// int i = 0;
	// int j = 0;
	// int address[21];
	// address[0] = 0;
	// address[1] = 8;
	// address[2] = 16;
	// address[3] = 24;
	// address[4] = 32;
	// address[5] = 40;
	// address[6] = 48;
	// address[7] = 56;
	// address[8] = 64;
	// address[9] = 72;
	// address[10] = 80;
	// address[11] = 88;
	// address[12] = 96;
	// address[13] = 104;
	// address[14] = 112;
	// address[15] = 120;
	// address[16] = 0;
	// address[17] = 8;
	// address[18] = 16;
	// address[19] = 24;
	// address[20] = 32;

	// int tail = 0;
	// void * cache = createAndInitialize(8, 64, 2);
	// Cache * cashay = (Cache *) cache;
	// int retval = -1;
	// for (i = 0; i < 21; i++) {
	// 	j = 0;
	// 	retval = accessCache(cache, address[i]);
	// 	printf("%d: Hit? %d\n", address[i], retval);
	// 	struct Node *next = cashay->assocCache->head;
	// 	while(next != NULL){
	// 		printf("assocCache[%d]: %d *** ", j, next->value);
	// 		if (cashay->assocCache->tail == next) {
	// 			tail = j;
	// 		}
 //      next = next->next;
 //      j++;
	// 	}
	// 	printf(" and the tail is at: %d\n", tail);
	// }

	int x1 = bitwise_equals(4, 4);
	int x2 = bitwise_equals(23, 8);
	int x3 = bitwise_equals(1024, 1024);
	int x4 = bitwise_equals(15, 15);
	int x5 = bitwise_equals(500, 5);

	printf("Calculated: %d, Expected: %d\n", x1, 1);
	printf("Calculated: %d, Expected: %d\n", x2, 0);
	printf("Calculated: %d, Expected: %d\n", x3, 1);
	printf("Calculated: %d, Expected: %d\n", x4, 1);
	printf("Calculated: %d, Expected: %d\n", x5, 0);

	return 0;
}