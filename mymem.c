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

struct memoryList {
    // doubly-linked list
    struct memoryList *last_node;
    struct memoryList *next_node;

    int size;            // How many bytes in this block?
    char alloc;          // 1 if this block is allocated,
    // 0 if this block is free.
    void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;
static struct memoryList *tail;


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

void initmem(strategies strategy, size_t sz)
{
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

    head = next;

}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1
 */


/* Inspiration er tager fra geeks for geeks https://www.geeksforgeeks.org/doubly-linked-list/ */
void insertNewNodeAfter(struct memoryList *givenNode, size_t givenSize, void *givenPtr, char givenAlloc){
    
    if(givenNode == NULL){
        return;
    }

    /* Her bliver den nye node alloceret i vores hukommelse.  */
    struct memoryList *node = (struct node*)malloc(sizeof(*node));

    /* Her sætter vi den nye nods parameter */
    node -> size = givenNode -> size - givenSize;
    node -> alloc = givenAlloc;
    node -> ptr = &givenPtr + givenSize;

    /* Her siger jeg at den nye next_node skal pege på den node som den forrige next_node pegede på.  */ 
    node -> next_node = givenNode -> next_node;

    /* Her siger jeg så at den ny node skal pege på den forrige node gennem last node. */
    node -> last_node = givenNode;

    /* Her siger jeg så at den forriges nodes next_node skal nu pege på den nye node som lige er blevet lavet */ 
    givenNode -> next_node = node;

    /* Her siger jeg at den nye nodes next_node's last_node skal være lige med node. */
    node -> next_node -> last_node = node;

    /* Dette if statment sætter vores currentNode/givenNode til at være lig med den node som vi lavede vores ny node ud fra, så vi kan begynde
    derfra igen næste gang. */

    next = node;
}

/* For at slette en node er der blevet taget inspiration fra geeks for geeks https://www.geeksforgeeks.org/delete-a-node-in-a-doubly-linked-list/ */
void deleteNode(struct memoryList *head, struct memoryList *delNode){
    
    /* Først bliver der lavet en base case*/
    if(head == NULL || delNode == NULL){
        return;
    }

    /* Herefter kigger vi på om det er head node der skal slettes */ 
    if(head == delNode){
        head = delNode -> next_node;
    }

    /* Her bliver der en node slettet hvis den næste node ikker er NULL */
    if(delNode -> next_node != NULL){
        delNode -> next_node -> last_node = delNode -> last_node;
    } 

    /* Her bliver der en node slettet hvis den tidligere node ikke er NULL */
    if(delNode -> last_node != NULL){
        delNode -> last_node -> next_node = delNode -> next_node;
    }

    free(delNode);
}

void *mymalloc(size_t requested)
{
    assert((int)myStrategy > 0);

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
	  struct memoryList *CurrentNode = next;

		do{
			if(next -> size >= requested && next -> alloc != 1){
				insertNewNodeAfter(CurrentNode, requested, CurrentNode ->ptr, CurrentNode ->alloc);
				CurrentNode -> size = requested;
				CurrentNode -> alloc = 1;

				return CurrentNode -> ptr;

			}else{
				if((next -> next_node == NULL) && next != head){
					tail = next;
					next = head;
				}else{
					next = CurrentNode -> next_node;
				}
			}

			CurrentNode -> next_node = CurrentNode;

		}while (next -> next_node != NULL);

		return CurrentNode -> ptr;
	}
    
}


/* Frees a block of memory previously allocated by mymalloc. */
/*  */ 
void myfree(void* node)
{
    struct memoryList *temp = head;

    while(temp->ptr == node) {
        
        temp->alloc = 0;

            /* Her kigger vi på om den node som vi vil frigøre's nabo's alloc også er lig med 0. Hvis den er skal de 2 nodes ligges sammen.  */
        if ((temp->last_node != NULL) && (temp->last_node->alloc == 0)) {
            temp->last_node->size += temp->size;
            /* Siden temp -> last nu skal fjernes skal dens next pointer sættes til at være tmp -> next */
            deleteNode(head,temp);
			break;

        } else if (temp->next_node != NULL) {
            temp = temp->next_node;

        } else {
            break;
        }
	}
    

    while (temp -> ptr == node){
        
        temp -> alloc = 0;

        if((temp -> next_node != NULL) && (temp -> next_node -> alloc = 0)){
            temp -> next_node -> size += temp -> size;

            deleteNode(head,temp);
			break;
        }
        else if(temp -> last_node != NULL){
            temp = temp -> last_node;

        } else {
            break;
        }
	}
}


/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the
 * memory pool this module manages via initmem/mymalloc/myfree.
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
    return 0;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
    return 0;
}

/* Number of non-allocated bytes */
int mem_free()
{
    return 0;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
    return 0;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
    return 0;
}

char mem_is_alloc(void *ptr)
{
    return 0;
}

/*
 * Feel free to use these functions, but do not modify them.
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
    return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
    return mySize;
}


// Get string name for a strategy.
char *strategy_name(strategies strategy)
{
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
strategies strategyFromString(char * strategy)
{
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
void print_memory()
{
	//TODO: Husk at ændre dette kopieret kode!!!
	printf("Memory List {\n");
  /* Iterate over memory list */
  struct memoryList* index = head;
  do {
    printf("\tBlock %p,\tsize %d,\t%s\n",
           index->ptr,
           index->size,
           (index->alloc ? "[ALLOCATED]" : "[FREE]"));
  } while((index = index->next) != head);
  printf("}\n");
    return;
}

/* Use this function to track memory allocation performance.
 * This function does not depend on your implementation,
 * but on the functions you wrote above.
 */
void print_memory_status()
{
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
