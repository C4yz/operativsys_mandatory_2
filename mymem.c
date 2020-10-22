#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>

#define TRUE 1
/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

typedef struct memoryList {
    // doubly-linked list
    struct memoryList *last_node;
    struct memoryList *next_node;

    int size;            // How many bytes in this block?
    char alloc;          // 1 if this block is allocated,
    // 0 if this block is free.
    void *ptr;           // location of block in memory pool.
} memoryList;

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static memoryList *head;
static memoryList *next;
static memoryList *tail;


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz){
    myStrategy = strategy;

    /* all implementations will need an actual block of memory to use */
    mySize = sz;

    if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

    /* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
    if(head != NULL){
        free(head);
    }

    if(next != NULL){
        free(next);
    }

    myMemory = malloc(sz);

    head = malloc(sizeof(struct memoryList));

    head -> ptr = myMemory;

    head -> alloc = 0;

    head -> size = sz;

    head -> next_node = NULL;

    head -> last_node = NULL;

    /* TODO: Initialize memory management structure. */

    next = head;

}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1
 */


/* Inspiration er tager fra geeks for geeks https://www.geeksforgeeks.org/doubly-linked-list/ */
void insertNewNodeAfter(memoryList *trav, size_t requested, void *travPtr, char travAlloc){
    
    if(trav == NULL){
        return;
    }
    
    /* Her bliver den nye node alloceret i vores hukommelse.  */
    memoryList *newNode = malloc(sizeof(memoryList));


    /* Her sætter vi den nye nods parameter */
    newNode -> size = trav -> size - requested;
    newNode -> alloc = travAlloc;
    newNode -> ptr = travPtr + requested;

    /* Her siger jeg at den nye next_node skal pege på den node som den forrige next_node pegede på.  */ 
    newNode -> next_node = trav -> next_node;

    /* Her siger jeg så at den ny node skal pege på den forrige node gennem last node. */
    newNode -> last_node = trav;

    /* Her siger jeg så at den forriges nodes next_node skal nu pege på den nye node som lige er blevet lavet */ 
    trav -> next_node = newNode;

    if(trav -> next_node != NULL){
        /* Her siger jeg at den nye nodes next_node's last_node skal være lige med node. */
        trav -> next_node -> last_node = newNode;
    }

    /* Dette if statment sætter vores currentNode/trav til at være lig med den node som vi lavede vores ny node ud fra, så vi kan begynde
    derfra igen næste gang. */
    next = newNode;
}

void *mymalloc(size_t requested){

    assert((int)myStrategy > 0);

    memoryList *trav = next;

	switch (myStrategy){
	  case NotSet: 
	            return NULL;
	  case First:
	            return NULL;
	  case Best:
	            return NULL;
	  case Worst:
	            return NULL;
	  case Next:

            do{
                if(trav -> next_node != NULL){
                    trav = trav -> next_node;
                }else{
                    trav = head;
                }
                //puts("hej");

            }while (trav -> size < requested && trav -> alloc != 1);
                
            insertNewNodeAfter(trav, requested, trav ->ptr, trav ->alloc);
            trav -> size = requested;
            trav -> alloc = 1;
                
            if(trav -> next_node != NULL){
                next = trav -> next_node;
            }else{
                next = head;
            }
            //next = next -> next_node;
    }
    return trav -> ptr;
}

memoryList* mergeNodes(memoryList* lastNode, memoryList* nextNode){

    lastNode -> next_node = nextNode -> next_node;
    lastNode -> size = lastNode -> size + nextNode -> size;
    lastNode -> alloc = 0;
    lastNode -> ptr = nextNode -> ptr;

    if(nextNode -> next_node != NULL){
        nextNode -> next_node -> last_node = lastNode;
    }

    free(nextNode);
    return lastNode;
}
/* Frees a block of memory previously allocated by mymalloc. */
/*  */ 
void myfree(void* node){
    memoryList *trav = head;

    do{
        if(trav -> ptr == node){
            trav -> alloc = 0;
            break;
        }

        trav = trav -> next_node;

    }while(trav != NULL);
    
    if((trav -> last_node != NULL) && (trav -> last_node -> alloc == 0)){
        trav = mergeNodes(trav -> last_node, trav);
    }

    if((trav -> next_node != NULL) && (trav -> next_node -> alloc == 0)){
        trav = mergeNodes(trav, trav -> next_node);
    }

    return;
}


/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the
 * memory pool this module manages via initmem/mymalloc/myfree.
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes(){
    int freeAlloc = 0;

    memoryList *trav = head;
     
    do{
        if(trav -> alloc == 0){
            freeAlloc++;
        }

        trav = trav -> next_node;

    }while(trav -> next_node != NULL);

    return freeAlloc;
}

/* Get the number of bytes allocated */
int mem_allocated(){
    int allocated = 0;

    memoryList *trav = head;

    do{
        if(trav -> alloc == 1){
            allocated++;
        }

        trav = trav -> next_node;

    }while(trav -> next_node != NULL);

    return allocated;
}

/* Number of non-allocated bytes */
int mem_free(){
    int memoryNotAlloc = 0;

    memoryList *trav = head;

    do{
        if(trav -> alloc == 0){
            memoryNotAlloc += trav -> size;
        }
        
        trav = trav -> next_node;

    }while(trav -> next_node != NULL);
    return memoryNotAlloc;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free(){
    return 0;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size){
    return 0;
}

char mem_is_alloc(void *ptr){
    return 0;
}

/*
 * Feel free to use these functions, but do not modify them.
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool(){
    return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total(){
    return mySize;
}


// Get string name for a strategy.
char *strategy_name(strategies strategy){
    switch (strategy)
    {
        case Best:
            return "best";
        case Worst:
            return "worst";
        case First:
            return "first";
        case Next:
            return "next";
        default:
            return "unknown";
    }
}

// Get strategy from name.
strategies strategyFromString(char * strategy){
    if (!strcmp(strategy,"best"))
    {
        return Best;
    }
    else if (!strcmp(strategy,"worst"))
    {
        return Worst;
    }
    else if (!strcmp(strategy,"first"))
    {
        return First;
    }
    else if (!strcmp(strategy,"next"))
    {
        return Next;
    }
    else
    {
        return 0;
    }
}


/*
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory(){
	//TODO: Husk at ændre dette kopieret kode!!!

    printf("Memory List {\n");
    /* Iterate over memory list */
    struct memoryList* index = head;
    do {
        printf("\tNode %p,\tsize %d,\t%s\n",
            index->ptr,
            index->size,
            (index->alloc ? "[ALLOCATED]" : "[FREE]"));
    } while((index = index->next_node) != NULL);
    printf("}\n");
}

/* Use this function to track memory allocation performance.
 * This function does not depend on your implementation,
 * but on the functions you wrote above.
 */
void print_memory_status(){
    printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
    printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
    printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */

void try_mymem(int argc, char **argv) {
    strategies strat;
    void *a, *b, *c, *d, *e;
    if(argc > 1)
        strat = strategyFromString(argv[1]);
    else
        strat = First;


    /* A simple example.
       Each algorithm should produce a different layout. */

    initmem(strat,500);

    a = mymalloc(100);
    b = mymalloc(100);
    c = mymalloc(100);
    myfree(b);
    d = mymalloc(50);
    myfree(a);
    e = mymalloc(25);

    print_memory();
    print_memory_status();

}
