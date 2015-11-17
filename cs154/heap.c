#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "functions.h"

typedef struct pipelineinstruction {
    double index;
    InstInfo * instruction;
} pipelineInstruction;

struct heapStruct {
    pipelineInstruction **heaparray;
    int capacity;
    int size;
};

// these are the different functions you need to write
int load(char *filename);
void fetch(InstInfo *);
void decode(InstInfo *);
void execute(InstInfo *);
void memory(InstInfo *);
void writeback(InstInfo *);

// heap forward declarations
struct heapStruct *initHeap();
//struct heapStruct * initHeapfromArray(int* values, int length);
void heapify(struct heapStruct *h);
void percolateDown(struct heapStruct *h, int index);
void percolateUp(struct heapStruct *h, int index);
//void insert(struct heapStruct *h, int value);
//int removeMin(struct heapStruct *h);
void insert(struct heapStruct *h, pipelineInstruction *value);
pipelineInstruction *removeMin(struct heapStruct *h);
void printHeap(struct heapStruct *h);
void swap(struct heapStruct *h, int index1, int index2);
//int minimum(int a, int indexa, int b, int indexb);
int minimum(pipelineInstruction *a, int indexa, pipelineInstruction *b, int indexb);
void freeHeap(struct heapStruct *h);
//void sort(int values[], int length);
pipelineInstruction **heapToArray(struct heapStruct *h);

//A simple test ripped straight from this hoes code
int main() {

    int i;
    int vals[10];
    struct heapStruct *h;
    h = initHeap();

    // Test out individual insert/removes (because 5 elements)
    //Make 5 different instructions (like a boss)

    pipelineInstruction *tempPipeInst;
    tempPipeInst = (pipelineInstruction*)(malloc(sizeof(pipelineInstruction)));
    tempPipeInst->instruction = (InstInfo*)(malloc(sizeof(InstInfo)));
    tempPipeInst->index = 0;
    sprintf(tempPipeInst->instruction->string,"add");
    pipelineInstruction *tempPipeInst1;
    tempPipeInst1 = (pipelineInstruction*)(malloc(sizeof(pipelineInstruction)));
    tempPipeInst1->instruction = (InstInfo*)(malloc(sizeof(InstInfo)));
    tempPipeInst1->index = 1;
    sprintf(tempPipeInst1->instruction->string,"and");
    pipelineInstruction *tempPipeInst2;
    tempPipeInst2 = (pipelineInstruction*)(malloc(sizeof(pipelineInstruction)));
    tempPipeInst2->instruction = (InstInfo*)(malloc(sizeof(InstInfo)));
    tempPipeInst2->index = 2;
    sprintf(tempPipeInst2->instruction->string,"lol");
    pipelineInstruction *tempPipeInst3;
    tempPipeInst3 = (pipelineInstruction*)(malloc(sizeof(pipelineInstruction)));
    tempPipeInst3->instruction = (InstInfo*)(malloc(sizeof(InstInfo)));
    tempPipeInst3->index = 3;
    sprintf(tempPipeInst3->instruction->string,"poop");
    pipelineInstruction *tempPipeInst4;
    tempPipeInst4 = (pipelineInstruction*)(malloc(sizeof(pipelineInstruction)));
    tempPipeInst4->instruction = (InstInfo*)(malloc(sizeof(InstInfo)));
    tempPipeInst4->index = 4;
    sprintf(tempPipeInst4->instruction->string,"ur mom");
    pipelineInstruction *tempPipeInst5;
    tempPipeInst5 = (pipelineInstruction*)(malloc(sizeof(pipelineInstruction)));
    tempPipeInst5->instruction = (InstInfo*)(malloc(sizeof(InstInfo)));
    tempPipeInst5->index = 2.5;
    sprintf(tempPipeInst5->instruction->string,"poopie");

    //Ought to be an assortment of NOOPs
    printf("Initial Heap\n");
    printHeap(h);


    //runs a remove and then an insert, becuase this is the necessary order, I believe
    //for the pipeline to safely execute. Kevin, you would know more, I think.
    removeMin(h);
    insert(h, tempPipeInst);
    printf("First Insert\n");
    printHeap(h);
    heapToArray(h);
    removeMin(h);
    insert(h, tempPipeInst1);
    printf("Second Insert\n");
    printHeap(h);
    heapToArray(h);
    removeMin(h);
    insert(h, tempPipeInst2);
    printf("Third Insert\n");
    printHeap(h);
    heapToArray(h);
    removeMin(h);
    insert(h, tempPipeInst3);
    printf("Fourth Insert\n");
    printHeap(h);
    heapToArray(h);
    removeMin(h);
    insert(h, tempPipeInst4);
    printf("Final Insert\n");
    printHeap(h);
    heapToArray(h);
    removeMin(h);
    insert(h, tempPipeInst5);
    printf("2.5 Insert\n");
    printHeap(h);
    heapToArray(h);

    for (i=0; i<5; i++) {
        printf("Delete element index %.2f\n",removeMin(h)->index);
        printHeap(h);
        heapToArray(h);
    }
    //freeHeap(h);
    
    // Test out array initialization.
    // vals[0] = 12, vals[1] = 3, vals[2] = 18, vals[3] = 14, vals[4] = 5;
    // vals[5] = 9, vals[6] = 1, vals[7] = 7; vals[8] = 2, vals[9] = 13;
    
    // sort(vals, 10);
    
    // for (i=0; i<10; i++)
    //     printf("%d ", vals[i]);
    // printf("\n");
    return 0;
}


struct heapStruct* initHeap() {

    struct heapStruct* h;

    // Allocate space for the heap and set the size for an empty heap.
    h = (struct heapStruct*)(malloc(sizeof(struct heapStruct)));
    h->capacity = 5;
    h->heaparray = (pipelineInstruction**)malloc(sizeof(pipelineInstruction*)*(5)); //or 6?
    h->size = 0;


    //Initializes the pipeline heap with noops where noops are defined by pipelineInstructions with
    //instruction->NULL

    pipelineInstruction *tempPipeInst;
    int i = 0;
    for (i = 0; i< 5; i++){
        tempPipeInst = (pipelineInstruction*)(malloc(sizeof(pipelineInstruction)));
        tempPipeInst->index = -1;
        tempPipeInst->instruction = NULL;
        insert(h, tempPipeInst);
    }

    return h;
}

// Frees the struct pointed to by h.
void freeHeap(struct heapStruct *h) {
     free(h->heaparray);
     free(h);
}

// Initializes the heap using the first length number of items in the array
// // values.
// struct heapStruct * initHeapfromArray(int* values, int length) {

//     int i;
//     struct heapStruct* h;

//     h = (struct heapStruct*)(malloc(sizeof(struct heapStruct)));
//     // We allocate one extra slot, since slot 0 stays unused.
//     h->heaparray = (int*)malloc(sizeof(int)*(length+1));

//     // Just copy the values into our array.
//     for (i=1; i<=length; i++)
//         h->heaparray[i] = values[i-1];

//     // This is the number of values we copied.
//     h->size = length;

//     // This takes our random values and rearranges them into a heap.
//     heapify(h);
//     return h;
// }

// h points to a heap structure that has values inside of it, but isn't yet
// organized into a heap and does exactly that.
void heapify(struct heapStruct *h) {

    int i;

    // We form a heap by just running percolateDown on the first half of the
    // elements, in reverse order.
    for (i=h->size/2; i>0; i--)
        percolateDown(h, i);

}

// Runs percolate down on the heap pointed to by h on the node stored in index.
void percolateDown(struct heapStruct *h, int index) {

    int min;

    // Only try to percolate down internal nodes.
    if ((2*index+1) <= h->size) {

        // Find the minimum value of the two children of this node.
        min = minimum(h->heaparray[2*index], 2*index, h->heaparray[2*index+1], 2*index+1);

      // If this value is less than the current value, then we need to move
      // our current value down the heap.

        //ALEX CHANGE HERE FOR DECIDING VALUE
        if (h->heaparray[index]->index > h->heaparray[min]->index) {
            swap(h, index, min);

            // This part is recursive and allows us to continue percolating
            // down the element in question.
            percolateDown(h, min);
        }
    }

    // Case where our current element has exactly one child, a left child.
    else if (h->size == 2*index) {

        // Here we only compare the current item to its only child.
        // Clearly, no recursive call is needed since the child of this node
        // is a leaf.

        //ALEX CHANGE HERE FOR DECIDING VALUE
        if (h->heaparray[index]->index > h->heaparray[2*index]->index)
            swap(h, index, 2*index);
    }
  }

// Runs percolate up on the heap pointed to by h on the node stored in index.
void percolateUp(struct heapStruct *h, int index) {

    // Can only percolate up if the node isn't the root.
    if (index > 1) {

        // See if our current node is smaller in value than its parent.
        //ALEX CHANGE HERE FOR DECIDING VALUE
        if (h->heaparray[index/2]->index > h->heaparray[index]->index) {

            // Move our node up one level.
            swap(h, index, index/2);

            // See if it needs to be done again.
            percolateUp(h, index/2);
        }
    }
}

// Inserts value into the heap pointed to by h.

//ALEX CHANGE HERE FOR TYPE
void insert(struct heapStruct *h, pipelineInstruction *value) {

    // int* temp;
    // int* throwaway;
    // int i;

    // Our array is full, we need to allocate some new space!
    // if (h->size == h->capacity) {

    //     // We will double the size of the structure.
    //     h->capacity *= 2;

    //     // Allocate new space for an array.
    //     temp = (int*)malloc(sizeof(int)*h->capacity+1);

    //     // Copy all the items over.
    //     for (i=1; i<=h->capacity; i++)
    //         temp[i] = h->heaparray[i];

    //     // Move the pointer and free the old memory.
    //     throwaway = h->heaparray;
    //     h->heaparray = temp;
    //     free(throwaway);
    // }

    // Adjust all the necessary components of h, and then move the inserted
    // item into its appropriate location.
    h->size++;
    h->heaparray[h->size] = value;
    percolateUp(h, h->size);
}

//ALEX CHANGE HERE FOR TYPE
pipelineInstruction *removeMin(struct heapStruct *h) {

    //ALEX CHANGE HERE FOR TYPE
    pipelineInstruction *retval;

    // We can only remove an element, if one exists in the heap!
    if (h->size > 0) {

        // This is where the minimum is stored.
        retval = h->heaparray[1];

        // Copy the last value into this top slot.
        h->heaparray[1] = h->heaparray[h->size];

        // Our heap will have one fewer items.
        h->size--;

        // Need to let this value move down to its rightful spot in the heap.
        percolateDown(h, 1);

        // Now we can return our value.
        return retval;
    }

    // No value to return, indicate failure with a -1.
    else{
        //ALEX CHANGE HERE
        //I think we can use this as the place to check that the
        //heap is empty in write-back, guys
        return NULL;
    }
}

// For debugging purposes, lets us see what's in the heap.
void printHeap(struct heapStruct *h) {
    int i;

    for (i=1; i<=h->size; i++)
    	if (h->heaparray[i]->instruction != NULL)
	        printf("(%.2f %s) ", h->heaparray[i]->index, h->heaparray[i]->instruction->string);
	    else
	    	printf("(%.2f NOOP) ", h->heaparray[i]->index);
    if (h->size == 0)
    	printf("We're empty!!");
    printf("\n");
}

// Swaps the values stored in the heap pointed to by h in index1 and index2.
void swap(struct heapStruct *h, int index1, int index2) {
    //ALEX CHANGE HERE FOR TYPE
    pipelineInstruction *temp = h->heaparray[index1];
    h->heaparray[index1] = h->heaparray[index2];
    h->heaparray[index2] = temp;
}

// Returns indexa if a < b, and returns indexb otherwise.
int minimum(pipelineInstruction *a, int indexa, pipelineInstruction *b, int indexb) {

    // Return the value associated with a.

    //ALEX CHANGE HERE FOR DECIDING VALUE
    if (a->index < b->index)
        return indexa;

    // Return the value associated with b.
    else
        return indexb;
}

//Alex's fancy make-the-heap-always-index-the-same-function (actually
// just makes the thing pop into an array)
pipelineInstruction **heapToArray(struct heapStruct *h) {

     pipelineInstruction **ret = (pipelineInstruction**)malloc(sizeof(pipelineInstruction*)*(5));
     int i;
     int size = h->size;

     for (i=0; i<(size); i++) {
         ret[i] = removeMin(h);
     }

     for (i=0; i<(size); i++) {
         insert(h, ret[i]);
     }

    for (i=0; i<h->size; i++){
        if (ret[i]->instruction != NULL){
            printf("(%.2f %s) ", ret[i]->index, ret[i]->instruction->string);
        }
        else{
            printf("(%.2f NOOP) ", ret[i]->index);
        }
    }
    printf("\n");

     return ret;
}

// Runs a heap sort by creating a heap out of the values in the array, and then
// extracting those values one-by-one from the heap back into the array in the
// proper order.
// ALEX CHANGE! WE ARE NOT DOING HEAP SORT, SO THIS IS UNNECESSARY
// void sort(int values[], int length) {

//      struct heapStruct *h;
//      int i;

//      // Create a heap from the array of values.
//      h =  initHeapfromArray(values, length);
//      length = h->size;

//      // Remove these values from the heap one by one and store them back in the
//      // original array.
//      for (i=0; i<length; i++) {
//          values[i] = removeMin(h);
//      }
// }













