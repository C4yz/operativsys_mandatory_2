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

    if (myMemory != NULL){
        free(myMemory);
    }

    /* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
    if(head != NULL){
        memoryList* trav = head;

        while (trav != NULL)
        {
            free(trav -> last_node);
            trav = trav -> next_node;
        }

        free(trav);
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


/* Denne metode indsætter en ny node efter den valgte node.  */
void insertNewNodeAfter(memoryList *trav, size_t requested, void *travPtr){
    
    if(trav == NULL){
        return;
    }
    
    /* Her bliver den nye node alloceret i vores hukommelse.  */
    memoryList *newNode = malloc(sizeof(memoryList));

    /* Hvis vores nodes next_node ikke er NULL kan pointerne blive opdeateret */
    if(trav -> next_node != NULL){
        newNode -> next_node = trav -> next_node;
        trav -> next_node -> last_node = newNode;

        newNode -> last_node = trav;
        trav -> next_node = newNode;
    }
    /* Hvis vores nodes next_node er lig med null er det slutningen af vores liste og pointerne bliver opdateret */
    else{
        trav -> next_node = newNode;
        newNode -> last_node = trav;
        newNode -> next_node = NULL;
        //tail = newNode;
    }

    /* Her sætter vi den nye nods parameter */
    newNode -> size = trav -> size - requested;
    newNode -> alloc = 0;
    newNode -> ptr = travPtr + requested;

    /* Her sætter vi den gamle nodes parametre.  */
    trav -> alloc = 1;
    trav -> size = requested;
}

void *mymalloc(size_t requested){

    /* Her sikre vi os at der faktisk er plads til den efterspurgte bit størrelse */
    if(mem_largest_free < requested){
        return NULL;
    }

    assert((int)myStrategy > 0);

    /* Her laver vi en midlertidig node som hedder trav og siger den skal starte ved head. */
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

            /* Her finder vi den første node der passer ud fra vores next pointer */

            while (trav -> size <= requested && trav -> alloc != 0){
                if(trav -> next_node == NULL){
                    trav = head;
                }
                else if(trav -> next_node == next){
                    return NULL;
                }
                else{
                    trav = trav -> next_node;
                }
            }
                
            /* Når vi så har fundet vores node kigger vi på om størrelsen passer. */    
            if(trav -> size >= requested){
                
                /* Hvis størrelsen passer præcist skal dens allocering bare sættes til 1. */
                if(trav -> size == requested){
                    trav -> alloc = 1;
                    if(trav -> next_node == NULL){
                        next = head;
                    }
                    else{
                        next = trav; 
                    }
                    return trav -> ptr;
                }

                /* Vi sætter den nye node ind. */
                insertNewNodeAfter(trav, requested, trav -> ptr);

            }
            
            /* Her opdatere vi vores next pointer til det næste. Hvis den er ved slutningen bliver det sat til head ellers den næste. */
            if (trav -> next_node != NULL){
                next = trav -> next_node;
            }
            else{
                next = head;
            }

    }
    return trav -> ptr;
}

/* Denne metode bruges til at merge 2 nodes sammen hvis de begge har allocering 0. */
void* mergeNodes(memoryList* trav, memoryList* nodeToMerge){
    /* Først laver vi en ny node og opdatere størrelsen. */
    memoryList* temp = trav;

    trav -> last_node -> size += trav -> size;

    /* Hvis vores nodes next_node er lig med NULL skal vi opdatere på en måde frem for hvis den ikke er.  */
    if(trav -> next_node == NULL){
        trav -> last_node -> next_node = NULL;
    }
    else{
        trav -> last_node -> next_node = trav -> next_node;
        trav -> next_node -> last_node = trav -> last_node;
    }

    /* Her opdatere vi next pointeren hvis det er nødvendigt og det var den som skulle slettes. */
    if(next == temp){
        if(temp -> next_node == NULL){
            next = head;
        }
        else{
            next = nodeToMerge;
        }
    }

    /* Til sidst frigøre vi vores node fra hukommelsen. */
    free(temp);
}

/* Frees a block of memory previously allocated by mymalloc. */
/* Denne metode bruges til at finde den node som skal frigøres og sætte dens allocering til 0. */ 
void myfree(void* node){

    if(node == NULL){
        return;
    }

    /* Vi begynder fra toppen af listen. */
    memoryList *trav = head;

    /* Går den igennem indtil at vi finder den rigtige node. */
    do{
        if(trav -> ptr == node){
            trav -> alloc = 0;
            break;
        }

        trav = trav -> next_node;

    }while(trav != NULL);

    if(!trav){
        return;
    }

    /* Her kigger vi på om den node vi arbejder ud fra er head hvis ikke den er det og next_node også har allocering 0 kan de 2 slås sammen. */
    if(trav->last_node!=NULL){
        if((trav != head) && (trav -> last_node -> alloc == 0)){
            memoryList *firstTemp = trav -> last_node;
            mergeNodes(trav, trav -> last_node);
            trav = firstTemp;
        }
    }

    /* Dette er den samme situation som den forrige men bare for den næste node og om den er den sidste node i listen. */ 
    if(trav -> next_node != NULL){
        if(trav -> next_node -> alloc == 0){
            memoryList* secondTemp = trav -> next_node;
            mergeNodes(secondTemp, trav);
        }
    }

    return;
}


/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the
 * memory pool this module manages via initmem/mymalloc/myfree.
 */

/* De følgende metoder er meget beskrevet i opgave beskrivelsen og de er bygget op på den samme måde. De går igennem vores liste og leder efter den rigtige node. Når
den så har fundet den node bliver det talt op på en eller anden måde. Dette kan være i form af en counter eller bare at ligge størrelsen sammen. */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes(){

    int freeAlloc = 0;

    memoryList *trav = head;
    
    
    do{
        if(trav -> alloc == 0){
            freeAlloc++;
        }

        trav = trav -> next_node;

    }while(trav != NULL);

    return freeAlloc;
}

/* Get the number of bytes allocated */
int mem_allocated(){
    int allocated = 0;

    memoryList *trav = head;

    do{
        if(trav -> alloc == 1){
            allocated += trav -> size;
        }

        trav = trav -> next_node;

    }while(trav != NULL);

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

    }while(trav != NULL);
    return memoryNotAlloc;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free(){
    int largestNotAlloc = 0;

    memoryList *trav = head;

    do{
        if(trav -> alloc == 0 && trav -> size > largestNotAlloc){
            largestNotAlloc = trav -> size;
        }

        trav = trav -> next_node;

    }while(trav != NULL);

    return largestNotAlloc;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size){
    int numOfSmallFreeNodes = 0;

    memoryList *trav = head;

    do{
        if(trav -> alloc == 0 && trav -> size <= size){
            numOfSmallFreeNodes++;
        }

        trav = trav -> next_node;

    }while(trav != NULL);

    return numOfSmallFreeNodes;
}

char mem_is_alloc(void *ptr){

    memoryList * trav = head;

    do{
        if(trav -> ptr == ptr){
            if(trav -> alloc == 1){
                return trav -> alloc;
            }
        }

        trav = trav -> next_node;

    }while(trav != NULL);
    
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

    int i = 0;

    printf("Memory List {\n");
    /* Iterate over memory list */
    struct memoryList* trav = head;
    do {
        printf("\tNode %p,\tsize %d,\t%s\n",
            trav->ptr,
            trav->size,
            (trav->alloc ? "[ALLOCATED]" : "[FREE]"));
            trav = trav -> next_node;
            printf("%i\n",i);
            i++;
    } while(trav != NULL);
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
    d = mymalloc(100);
    e = mymalloc(100);
    myfree(b);
    myfree(d);
    myfree(c);


    print_memory();
    print_memory_status();
}