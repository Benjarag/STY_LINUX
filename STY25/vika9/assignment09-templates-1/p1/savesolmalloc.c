#define USE_REAL_SBRK 1

#if USE_REAL_SBRK
#define _GNU_SOURCE

#include <sys/mman.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "malloc.h"

/* Function to allocate heap. Do not modify.
 * This is a wrapper around the system call sbrk.
 * For initialization, you can call this function as allocHeap(NULL, size)
 *   -> It will allocate a heap of size <size> bytes and return a pointer to the start address
 * For enlarging the heap, you can later call allocHeap(heapaddress, newsize)
 *   -> heapaddress must be the address previously returned by allocHeap(NULL, size)
 *   -> newsize is the new size
 *   -> function will return NULL (no more memory available) or heapaddress (if ok)
 */

uint8_t *allocHeap(uint8_t *currentHeap, uint64_t size)
{               
        static uint64_t heapSize = 0;
        if( currentHeap == NULL ) {
                uint8_t *newHeap  = sbrk(size);
                if(newHeap)
                        heapSize = size;
                return newHeap;
        }
	uint8_t *newstart = sbrk(size - heapSize);
	if(newstart == NULL) return NULL;
	heapSize = size;
	return currentHeap;
}
#else
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include "malloc.h"
// This is a "fake" version that you can use on MacOS
// sbrk as used above is not available on MacOS
// and normal malloc allocation does not easily allow resizing the allocated memory
uint8_t *allocHeap(uint8_t *currentHeap, uint64_t size)
{
        static uint64_t heapSize = 0;
        if( currentHeap == NULL ) {
                uint8_t *newHeap  = malloc(10*size);
                if(newHeap)
                        heapSize = 10*size;
                return newHeap;
        }
	if(size <= heapSize) return currentHeap;
        return NULL;
}
#endif

#include <pthread.h>  // Add this for pthread mutex support

/* Global variable that indicates if debug is enabled or not */
int debug = 0;

static pthread_mutex_t alloc_mutex;

/*
 * This is the heap you should use.
 * (Initialized with a heap of size HEAP_SIZE) in initAllocator())
 */
uint8_t *_heapStart = NULL;
uint64_t _heapSize = 0;

/*
 * This should point to the first free block in memory.
 */
Block *_firstFreeBlock;

/*
 * Initializes the memory block. You don't need to change this.
 */
void initAllocator()
{
        _heapStart = allocHeap(NULL, HEAP_SIZE);
	_heapSize = HEAP_SIZE;
	printf("Heap start is %p\n", _heapStart);
	/* Add more initialization below, e.g. for the free block list */
	_firstFreeBlock = (Block *)_heapStart;
	_firstFreeBlock->size = HEAP_SIZE;
	_firstFreeBlock->next = NULL;

	pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&alloc_mutex, &attr);
}


/*
 * Gets the next block that should start after the current one.
 */
static Block *_getNextBlockBySize(const Block *current)
{
	Block *end = (Block*)(_heapStart + _heapSize);
	Block *next = (Block*)&current->data[current->size - HEADER_SIZE];

	assert(next <= end);
	return (next == end) ? NULL : next;
}

/*
 * Dumps the allocator. You should not need to modify this.
 */
void dumpAllocator()
{
	pthread_mutex_lock(&alloc_mutex);
	Block *current;
	/* Note: This sample code prints addresses relative to the beginning of the heap */

	/* Part a: all blocks, using size, and starting at the beginning of the heap */
	printf("All blocks:\n");
	current = (Block*) _heapStart;
	while (current) {
		printf("  Block starting at %" PRIuPTR ", size %" PRIu64 " (%s)\n",
			((uintptr_t)(void*)current - (uintptr_t)_heapStart),
			current->size,
			current->next == ALLOCATED_BLOCK_MAGIC ? "in use" : "free" );

		current = _getNextBlockBySize(current);
	}

	/* Part b: free blocks, using next pointer, and starting at _firstFreeBlock */
	printf("Free block list:\n");
	current = _firstFreeBlock;
	while (current) {
		printf("  Free block starting at %" PRIuPTR ", size %" PRIu64 "\n",
			((uintptr_t)(void*)current - (uintptr_t)_heapStart),
			current->size);

		current = current->next;
	}
	pthread_mutex_unlock(&alloc_mutex);
}

/*
 * Round the integer up to the block header size (16 Bytes).
 */
uint64_t roundUp(uint64_t n)
{
	return (n+0xF) & (~0xF);
}

/* Helper function that allocates a block (last two bullets of Assignment description)
 * takes as first argument the address of the next pointer that needs to be updated (!)
 */
static void *allocate_block(Block **update_next, Block *block, uint64_t new_size) {
	assert(block->size >= new_size);  // something is wrong if this is not the case

	if(block->size == new_size) {
		// exactly the right size
		// remove the free block from the free-list
		*update_next = block->next;

		// Mark block as allocated
		block->next = ALLOCATED_BLOCK_MAGIC;

		// return address of the data part
		return &(block->data[0]);
	}

	// Create new free block

	// Size of the new free block is the size of the current free block minutes the space needed for the new allocation
	uint64_t new_free_size = block->size - new_size;

	// Prepare the new allocated block
	// Don't update magic yet (we still need the next pointer)
	// block->magic = ALLOCATED_BLOCK_MAGIC;
	block->size = new_size;

	// Prepare the new free block
	Block *newfree = _getNextBlockBySize( block );
	newfree->size = new_free_size;
	newfree->next = block->next;
	block->next = ALLOCATED_BLOCK_MAGIC;

	// update the previous free pointer to the new free block
	*update_next = newfree;
	return &(block->data[0]);
}

void *my_malloc(uint64_t size)
{
	/* round the requested size up to the next larger multiple of 16, and add header */
	size = roundUp(size) + HEADER_SIZE;

	pthread_mutex_lock(&alloc_mutex);

	if (debug) {
		dumpAllocator();
	}

	/* Search the free list to find the first free block of memory that is large enough */
	Block *block = _firstFreeBlock;
	Block **prevptr = &_firstFreeBlock;

	uint64_t bestSize = 0;
	Block *bestBlock = NULL;
	Block **bestLink = NULL;
    	while (block) {
        	if (block->size >= size) {
            		if(bestBlock==NULL || block->size<bestSize) {
                		// Either we are a better fit, or we don't have 
                		bestSize = block->size;
				bestBlock = block;
				bestLink = prevptr;
			}
		}
		prevptr = &block->next;
		block = block->next;
	}

	/* free list is empty or there is no block that is large enough */
	if(bestBlock == NULL) {
		if(size > HEAP_SIZE - sizeof(Block)) {
			pthread_mutex_unlock(&alloc_mutex);
			return NULL; // never allocate more than that in one go...
		}
		/* Try to get more memory */
		uint8_t *ptr = allocHeap(_heapStart, _heapSize + HEAP_SIZE);
		if(!ptr) {
			pthread_mutex_unlock(&alloc_mutex);
			return NULL;
		}
		Block *newfree = (Block *)(_heapStart + _heapSize);
		newfree->size = HEAP_SIZE;
		newfree->next = NULL;
		*prevptr = newfree;
		_heapSize += HEAP_SIZE;

		// do allocation in new block
		bestBlock = newfree;
		bestLink = prevptr;
	}
	void *allocated = allocate_block(bestLink, bestBlock, size);
	
	if (debug) {
		dumpAllocator();
	}
	
	pthread_mutex_unlock(&alloc_mutex);
	return allocated;

}


/* Helper function to merge two freelist blocks.
 * Assume: block1 is at a lower address than block2
 * Does nothing if blocks are not neighbors (i.e. if block1 address + block1 size is not block2 address)
 * Otherwise, merges block by merging block2 into block1 (updates block1's size and next pointer
 */
void merge_blocks(Block *block1, Block *block2)
{
	if( (char *)block1 + block1->size != (char *)block2 ) {
		return;
	}
	assert(block1->next == block2); // This really should be the case :)
		

	block1->size += block2->size;
	block1->next = block2->next;
}


void my_free(void *address)
{
	// If address is NULL, do nothing and just return
	if(address==NULL) return;

	// Derive the allocation block from the address
	Block *block = (Block *)( (char *)address - HEADER_SIZE );

	pthread_mutex_lock(&alloc_mutex);

	// Insert block into freelist
	Block *freeblock = _firstFreeBlock;
	if(block < freeblock) { // insert at beginning
		block->next = _firstFreeBlock;
		_firstFreeBlock = block;	
		merge_blocks(block, block->next);
	} else {
		// blocks that are before our new free block
		while(freeblock->next < block && freeblock->next != NULL) {
			freeblock = freeblock->next;
		}
		// This if is not really necessary....
		if(freeblock->next == NULL) {  // append at the end
			block->next = NULL;
			freeblock->next = block;
		} else {
			// freeblock is before block, freeblock->next is after block, so insert in the middle
			block->next = freeblock->next;
			freeblock->next = block;
		}
		// merge with next block if neighbours
		// (block->next can be null, but then merge_blocks simply does nothing)
		merge_blocks(block, block->next);
		// merge with next block if neighbours
		merge_blocks(freeblock, block);
	}
	pthread_mutex_unlock(&alloc_mutex);
}


/* enable: 0 (false) to disable, 1 (true) to enable debug */
void enable_debug(int enable)
{
	debug = enable;
}
