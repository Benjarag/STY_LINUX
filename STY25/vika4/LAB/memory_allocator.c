#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct Block {
    uint64_t size;       // Total size of the block, including the header
    struct Block *next;  // Points to the next free block, or NULL if last
    uint8_t data[];      // Flexible array for block data
} Block;

// Function to get the next block using size information
static Block *getNextBlockBySize(const Block *current) {
    return (Block *)((uint8_t *)current + current->size);
}

// Function to iterate over all blocks using size-based navigation
void iterate_all_blocks(Block *heap_start, Block *heap_end) {
    Block *current = heap_start;
    while ((uint8_t *)current < (uint8_t *)heap_end) {
        printf("Block at: %p, size: %lu\n", (void *)current, current->size);
        current = getNextBlockBySize(current);
    }
}

// Function to iterate over free blocks using next pointer
void iterate_free_blocks(Block *free_list) {
    Block *current = free_list;
    while (current != NULL) {
        printf("Block at: %p, size: %llu\n", (void *)current, (unsigned long long)current->size);
        current = current->next;
    }
}

int main() {
    // Simulate heap memory allocation
    uint8_t memory[256];  // Simulated heap of 256 bytes

    // Create three blocks in the simulated memory
    Block *block1 = (Block *)memory;
    block1->size = 64;  // Block 1: 64 bytes
    block1->next = NULL;

    Block *block2 = getNextBlockBySize(block1);
    block2->size = 80;  // Block 2: 80 bytes
    block2->next = NULL;

    Block *block3 = getNextBlockBySize(block2);
    block3->size = 48;  // Block 3: 48 bytes
    block3->next = NULL;

    // Link free list (block1 -> block2 -> block3)
    block1->next = block2;
    block2->next = block3;

    // Set heap_end to point to just after the last byte of memory
    Block *heap_end = (Block *)((uint8_t *)block3 + block3->size);  // After the last block
        
    // Test iteration of all blocks
    printf("Iterating all blocks:\n");
    iterate_all_blocks(block1, heap_end);

    printf("\nIterating free blocks:\n");
    iterate_free_blocks(block1);

    return 0;
}
