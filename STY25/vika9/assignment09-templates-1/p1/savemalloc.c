#define USE_REAL_SBRK 1
#pragma GCC diagnostic ignored "-Wunused-function"

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
	heapSize += size;
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

	/* Add more initialization below, e.g. for the free block list */
	_firstFreeBlock = (Block *)_heapStart;
	_firstFreeBlock->size = HEAP_SIZE;
	_firstFreeBlock->next = NULL;
	// free_block_list = initialBlock;
	pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&alloc_mutex, &attr);
	// See lab tutorial
}


/*
 * Gets the next block that should start after the current one.
 */
static Block *_getNextBlockBySize(const Block *current)
{
    Block *end = (Block *)(_heapStart + _heapSize);
	// Block *next = (Block *)&current->data[current->size - HEADER_SIZE];
	Block *next = (Block *) ((uint8_t *)current + current->size);

	assert(next <= end);
	return (next == end) ? NULL : next;
	// See lab tutorial
}

/*
 * Dumps the allocator. You should not need to modify this.
 */
void dumpAllocator()
{
	pthread_mutex_lock(&alloc_mutex);
	Block *current = (Block *)_heapStart;
	while (current) {
		printf("Block at: %p, size: %lu\n", (void *)current, current->size);
		current = _getNextBlockBySize(current);
	}
	Block *curr = _firstFreeBlock;
	while (curr != NULL) {
        printf("Block at: %p, size: %llu\n", (void *)curr, (unsigned long long)curr->size);
		curr = curr->next;
	}
	printf("WE ARE IN DUMPALLOCATOR\n");
	pthread_mutex_unlock(&alloc_mutex);
	// See lab tutorial
}

/*
 * Round the integer up to the block header size (16 Bytes).
 */
uint64_t roundUp(uint64_t n)
{
	// See lab tutorial
	// Round up an integer to the nearest multiple of 16
	return (n + 15) & ~15;  // Add 15, then clear the last 4 bits
}
	
/* Helper function that allocates a block 
 * takes as first argument the address of the next pointer that needs to be updated (!)
 */
static void *allocate_block(Block **update_next, Block *block, uint64_t new_size)
{
	// (void)update_next;
	// (void)block;
	// (void)new_size;
	/* Not mandatory but possibly useful to implement this as a separate function
	 * called by my_malloc */

	printf("Allocating block at: %p with size: %lu\n", (void *)block, new_size);

	// exact fit: remove it from the list
	if (block->size == new_size) {
		printf("Exact fit found. Removing block at: %p\n", (void *)block);
		*update_next = block->next; // removes and points prev next pointer to the removed block next
	}
	else {
		Block *new_free_block = (Block *)((uint8_t *)block + new_size); // creates new free block from the best fit block and the required size
		new_free_block->size = block->size - new_size; // size to bestfit size - required size == remaining size
		new_free_block->next = block->next;

        printf("Splitting block at: %p into size: %lu, remaining free block at: %p with size: %lu\n", 
			(void *)block, new_size, (void *)new_free_block, new_free_block->size);

		*update_next = new_free_block; // put it at the same location
		block->size = new_size; // best fit block to the required size
	}
	block->next = NULL;
	return (void *)(block + 1);
}

void *my_malloc(uint64_t size)
{
	/* TODO: Implement */
	size = HEADER_SIZE + roundUp(size);
	
	// check if size exceeds HEAP_SIZE to avoid infinite extensions
    if (size > HEAP_SIZE) {
		printf("Requested size exceeds HEAP_SIZE. Returning NULL.\n");
        return NULL;
    }
	
	printf("Requesting allocation of size: %lu\n", size);
	
	// Now lock before accessing or modifying shared state.
	pthread_mutex_lock(&alloc_mutex);
	
	if (debug) {
		dumpAllocator();
	}

	Block *best_fit = NULL;
	Block **best_prev_next = &_firstFreeBlock;
	Block *prev = NULL;
	Block *current = _firstFreeBlock;

	while (current) {
        printf("Checking block at: %p with size: %lu\n", (void *)current, current->size);
		if (current->size >= size) { // large enough
			if (!best_fit || current->size < best_fit->size) { // smaller size than the best_fit size
                printf("New best fit found at: %p with size: %lu\n", (void *)current, current->size);
				// change current to best fit
				best_fit = current; 
				best_prev_next = prev ? &prev->next : &_firstFreeBlock; // if not prev then its the firstFreeBlock
			}
		}
		prev = current;
		current = current->next;
	}

	// no best fit block found
	if (!best_fit) {
		printf("No suitable block found. Attempting to extend the heap.\n");
		uint8_t *new_heap_start = allocHeap(_heapStart, _heapSize + HEAP_SIZE);
		if (new_heap_start == NULL) {
			printf("Failed to extend heap. Returning NULL.\n");
			pthread_mutex_unlock(&alloc_mutex);
			return NULL;
		}
		uint64_t original_heap_size = _heapSize;
		_heapSize += HEAP_SIZE;

		// creating a new block at the start of the newly allocated area
		Block *new_block = (Block *)(_heapStart + original_heap_size);
		new_block->size = HEAP_SIZE;
		new_block->next = NULL;

		// inserting new_block into the free list in address order
		Block **insert_pos = &_firstFreeBlock;
		while (*insert_pos != NULL && *insert_pos < new_block) {
			insert_pos = &(*insert_pos)->next;
		}
		new_block->next = *insert_pos;
		*insert_pos = new_block;

		printf("New free block added at: %p with size: %lu\n", (void *)new_block, new_block->size);

		// retry the allocation
		void *ptr = my_malloc(size);
		pthread_mutex_unlock(&alloc_mutex);
		return ptr;
	}
		
    printf("Best fit block selected at: %p with size: %lu\n", (void *)best_fit, best_fit->size);
	void *allocated = allocate_block(best_prev_next, best_fit, size);
	
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
static void merge_blocks(Block *block1, Block *block2)
{
	/* TODO: Implement */
	/* Note: Again this is not mandatory but possibly useful to put this in a separate
	 * function called by my_free */
	// checking if the blocks are adjacent
    if ((uint8_t*)block1 + block1->size == (uint8_t*)block2) {
        // merge the blocks by increasing the size of block1
        block1->size += block2->size;
        block1->next = block2->next;  // update the next pointer of block1
    }
}


void my_free(void *address)
{
	// (void)address;
	/* TODO: implement */
	if (address == NULL) {
        printf("my_free: Address is NULL, doing nothing.\n");
        return;  // do nothing if the address is NULL
    }

	pthread_mutex_lock(&alloc_mutex);
    // get the Block pointer from the address (which is located after the Block header)
    Block *block_to_free = (Block *)((uint8_t *)address - HEADER_SIZE);
    printf("my_free: Freeing block at address: %p with size: %lu\n", (void *)block_to_free, block_to_free->size);


    // insert the block into the free list in the correct position (sorted by address)
    Block *prev = NULL;
    Block *current = _firstFreeBlock;

    // find the correct position for the block in the free list
    while (current && current < block_to_free) {
        prev = current;
        current = current->next;
    }

    // insert the block into the free list
    if (prev == NULL) {
        // if we're inserting at the head of the free list
        block_to_free->next = _firstFreeBlock; // setting the next pointer of block to free to the head of the free list
        _firstFreeBlock = block_to_free; // update the head of the free list to be the block to free
        printf("my_free: Inserted block at the beginning of the free list.\n");
    } else {
        prev->next = block_to_free; // make the previous block point to the block to free
        block_to_free->next = current; // next pointer to be the current which is the block after
        printf("my_free: Inserted block after address: %p\n", (void *)prev);
    }

    // try to merge with previous block if it's free
    if (prev && (uint8_t *)prev + prev->size == (uint8_t *)block_to_free) {
        printf("my_free: Merging with previous block at address: %p\n", (void *)prev);
        merge_blocks(prev, block_to_free);
        block_to_free = prev;  // update block_to_free to the merged block
    }

    // try to merge with next block if it's free
    if (current && (uint8_t *)block_to_free + block_to_free->size == (uint8_t *)current) {
        printf("my_free: Merging with next block at address: %p\n", (void *)current);
        merge_blocks(block_to_free, current);
    }

    printf("my_free: Completed freeing block at address: %p\n", (void *)block_to_free);
	pthread_mutex_unlock(&alloc_mutex);
}

/* enable: 0 (false) to disable, 1 (true) to enable debug */
void enable_debug(int enable)
{
	debug = enable;
}

