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

uint8_t *allocHeap(uint8_t *currentHeap, uint64_t size)
{               
    static uint64_t heapSize = 0;
    if (currentHeap == NULL) {
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

uint8_t *_heapStart = NULL;
uint64_t _heapSize = 0;
Block *_firstFreeBlock;

static int allocationStrategy = ALLOC_BESTFIT;
/* Next-fit pointer (used when strategy is ALLOC_NEXTFIT) */
static Block *_next_fit_cursor = NULL;



void initAllocator()
{
    _heapStart = allocHeap(NULL, HEAP_SIZE);
    _heapSize = HEAP_SIZE;
    _firstFreeBlock = (Block *)_heapStart;
    _firstFreeBlock->size = HEAP_SIZE;
    _firstFreeBlock->next = NULL;
}

static Block *_getNextBlockBySize(const Block *current)
{
    Block *end = (Block *)(_heapStart + _heapSize);
    Block *next = (Block *) ((uint8_t *)current + current->size);
    assert(next <= end);
    return (next == end) ? NULL : next;
}

void dumpAllocator()
{
    Block *current = (Block *)_heapStart;
    while (current) {
        printf("Block at: %p, size: %lu\n", (void *)current, current->size);
        current = _getNextBlockBySize(current);
    }
    Block *curr = _firstFreeBlock;
    while (curr != NULL) {
        printf("Free block at: %p, size: %llu\n", (void *)curr, (unsigned long long)curr->size);
        curr = curr->next;
    }
    printf("WE ARE IN DUMPALLOCATOR\n");
}

uint64_t roundUp(uint64_t n)
{
    return (n + 15) & ~15;
}
	
static void *allocate_block(Block **update_next, Block *block, uint64_t new_size)
{
    printf("Allocating block at: %p with size: %lu\n", (void *)block, new_size);
    if (block->size == new_size) {
        printf("Exact fit found. Removing block at: %p\n", (void *)block);
        *update_next = block->next;
    }
    else {
        Block *new_free_block = (Block *)((uint8_t *)block + new_size);
        new_free_block->size = block->size - new_size;
        new_free_block->next = block->next;
        printf("Splitting block at: %p into allocated size: %lu, remaining free block at: %p with size: %lu\n", 
            (void *)block, new_size, (void *)new_free_block, new_free_block->size);
        *update_next = new_free_block;
        block->size = new_size;
    }
    block->next = NULL;
    return (void *)(block + 1);
}

void *my_malloc(uint64_t size)
{
    size = HEADER_SIZE + roundUp(size);
    if (size > HEAP_SIZE) {
        printf("Requested size exceeds HEAP_SIZE. Returning NULL.\n");
        return NULL;
    }
	
    printf("Requesting allocation of size: %lu\n", size);
    Block *selected_block = NULL;
    Block **selected_prev_next = NULL;
    Block *prev = NULL;
    Block *current = NULL;

    switch(allocationStrategy)
    {
        case ALLOC_BESTFIT:
        {
            Block *best_fit = NULL;
            Block **best_prev_next = &_firstFreeBlock;
            prev = NULL;
            current = _firstFreeBlock;
            while (current) {
                printf("Checking block at: %p with size: %lu (BESTFIT)\n", (void*)current, current->size);
                if (current->size >= size) {
                    if (!best_fit || current->size < best_fit->size) {
                        best_fit = current;
                        best_prev_next = prev ? &prev->next : &_firstFreeBlock;
                        printf("New best fit found at: %p with size: %lu\n", (void*)current, current->size);
                    }
                }
                prev = current;
                current = current->next;
            }
            selected_block = best_fit;
            selected_prev_next = best_prev_next;
            break;
        }
        case ALLOC_FIRSTFIT:
        {
            prev = NULL;
            current = _firstFreeBlock;
            while (current) {
                printf("Checking block at: %p with size: %lu (FIRSTFIT)\n", (void*)current, current->size);
                if (current->size >= size) {
                    selected_block = current;
                    selected_prev_next = prev ? &prev->next : &_firstFreeBlock;
                    printf("First fit found at: %p with size: %lu\n", (void*)current, current->size);
                    break;
                }
                prev = current;
                current = current->next;
            }
            break;
        }
        case ALLOC_NEXTFIT:
        {
            if (_next_fit_cursor == NULL) {
                _next_fit_cursor = _firstFreeBlock;
            }
            int has_circled = 0;
            Block *starting_next_fit_address = _next_fit_cursor;  // store starting point
            
            Block *current = _next_fit_cursor;
            Block *prev = NULL;
            selected_block = NULL;
            selected_prev_next = NULL;
            
            while (!has_circled) {
                printf("Checking block at: %p with size: %lu\n", (void *)current, current->size);
                if (current->size >= size) { // large enough
                    selected_block = current; // select the current block
                    selected_prev_next = (prev == NULL) ? &_firstFreeBlock : &prev->next; // set the selected_prev_next to the prev next pointer, if not prev then its the firstFreeBlock
                    printf("First fit block selected at: %p with size: %lu\n", (void *)current, current->size);
                    break;
                }
                prev = current; // update prev to current
                current = current->next; // update current to next
                if (current == NULL) {
                    current = _firstFreeBlock;
                }
                if (current == starting_next_fit_address) {
                    has_circled = 1;
                }
            }
            break;
        }
        case ALLOC_WORSTFIT:
        {
            Block *worst_fit = NULL;
            Block **worst_prev_next = &_firstFreeBlock;
            prev = NULL;
            current = _firstFreeBlock;
            while (current) { 
                printf("Checking block at: %p with size: %lu (WORSTFIT)\n", (void*)current, current->size);
                if (current->size >= size) { // Large enough
                    if (!worst_fit || current->size > worst_fit->size) { // larger size than the selected block size
                        worst_fit = current;
                        worst_prev_next = prev ? &prev->next : &_firstFreeBlock; // if not prev then its the firstFreeBlock
                        printf("New worst fit candidate at: %p with size: %lu\n", (void*)current, current->size);
                    }
                }
                prev = current;
                current = current->next;
            }
            selected_block = worst_fit;
            selected_prev_next = worst_prev_next;
            break;
        }
    }

    if (!selected_block) {
        printf("No suitable block found. Attempting to extend the heap.\n");
        uint8_t *new_heap_start = allocHeap(_heapStart, _heapSize + HEAP_SIZE);
        if (new_heap_start == NULL) {
            printf("Failed to extend heap. Returning NULL.\n");
            return NULL;
        }
        uint64_t original_heap_size = _heapSize;
        _heapSize += HEAP_SIZE;
        Block *new_block = (Block *)(_heapStart + original_heap_size);
        new_block->size = HEAP_SIZE;
        new_block->next = NULL;
        Block **insert_pos = &_firstFreeBlock;
        while (*insert_pos != NULL && *insert_pos < new_block) {
            insert_pos = &(*insert_pos)->next;
        }
        new_block->next = *insert_pos;
        *insert_pos = new_block;
        printf("New free block added at: %p with size: %lu\n", (void *)new_block, new_block->size);
        if (allocationStrategy == ALLOC_NEXTFIT) {
            _next_fit_cursor = _firstFreeBlock;
            printf("Reset next_fit_cursor to _firstFreeBlock after heap extension.\n");
        }
        return my_malloc(size);
    }
		
    printf("Selected block at: %p with size: %lu for allocation.\n", (void *)selected_block, selected_block->size);
    // make a variable ret for the allocate_block call here because we need to update the next_fit_cursor after this
    void *ret = allocate_block(selected_prev_next, selected_block, size); 

    if (allocationStrategy == ALLOC_NEXTFIT) {
        if (*selected_prev_next != NULL)
            _next_fit_cursor = *selected_prev_next;
        else
            _next_fit_cursor = _firstFreeBlock;
        printf("Updated next_fit_cursor to: %p\n", (void *)_next_fit_cursor);
    }
    return ret;
}

static void merge_blocks(Block *block1, Block *block2)
{
    if ((uint8_t*)block1 + block1->size == (uint8_t*)block2) {
        block1->size += block2->size;
        block1->next = block2->next;
    }
}

void my_free(void *address)
{
    if (address == NULL) {
        printf("my_free: Address is NULL, doing nothing.\n");
        return;
    }
    Block *block_to_free = (Block *)((uint8_t *)address - HEADER_SIZE);
    printf("my_free: Freeing block at address: %p with size: %lu\n", (void *)block_to_free, block_to_free->size);
    Block *prev = NULL;
    Block *current = _firstFreeBlock;
    while (current && current < block_to_free) {
        prev = current;
        current = current->next;
    }
    if (prev == NULL) {
        block_to_free->next = _firstFreeBlock;
        _firstFreeBlock = block_to_free;
        printf("my_free: Inserted block at the beginning of the free list.\n");
    } else {
        prev->next = block_to_free;
        block_to_free->next = current;
        printf("my_free: Inserted block after address: %p\n", (void *)prev);
    }
    if (prev && (uint8_t *)prev + prev->size == (uint8_t *)block_to_free) {
        printf("my_free: Merging with previous block at address: %p\n", (void *)prev);
        merge_blocks(prev, block_to_free);
        block_to_free = prev;
    }
    if (current && (uint8_t *)block_to_free + block_to_free->size == (uint8_t *)current) {
        printf("my_free: Merging with next block at address: %p\n", (void *)current);
        merge_blocks(block_to_free, current);
    }
    printf("my_free: Completed freeing block at address: %p\n", (void *)block_to_free);
    if (allocationStrategy == ALLOC_NEXTFIT) {
        _next_fit_cursor = block_to_free;
        printf("my_free: Updated next_fit_cursor to: %p after free/merge.\n", (void*) _next_fit_cursor);
    }
}

void setAllocationStrategy(AllocType type)
{
    allocationStrategy = type;
    printf("Allocation strategy set to: ");
    switch(type)
    {
        case ALLOC_BESTFIT:
            printf("BESTFIT\n");
            break;
        case ALLOC_FIRSTFIT:
            printf("FIRSTFIT\n");
            break;
        case ALLOC_NEXTFIT:
            printf("NEXTFIT\n");
            _next_fit_cursor = _firstFreeBlock;
            break;
        case ALLOC_WORSTFIT:
            printf("WORSTFIT\n");
            break;
        default:
            printf("UNKNOWN (defaulting to BESTFIT)\n");
            allocationStrategy = ALLOC_BESTFIT;
            break;
    }
}

MallocStat getAllocStatistics()
{
    MallocStat stat;
    stat.nFree = 0;
    uint64_t totalFree = 0;
    stat.largestFree = 0;
    Block *current = _firstFreeBlock;
    while(current) {
        stat.nFree++;
        totalFree += current->size;
        if (current->size > stat.largestFree)
            stat.largestFree = current->size;
        current = current->next;
    }
    if (stat.nFree > 0)
        stat.avgFree = totalFree / stat.nFree;
    else
        stat.avgFree = 0;
    printf("Allocation Statistics: nFree=%u, avgFree=%u, largestFree=%u\n", stat.nFree, stat.avgFree, stat.largestFree);
    return stat;
}