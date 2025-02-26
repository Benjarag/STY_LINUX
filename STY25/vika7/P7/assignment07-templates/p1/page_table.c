#pragma GCC diagnostic ignored "-Wunused-function"

#define _POSIX_C_SOURCE 200112L
#include "page_table.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// The pointer to the base directory.
// You can safely assume that this is set before any address conversion is done.
static PageDirectory *_cr3 = NULL;

void setPageDirectory(PageDirectory *directory)
{
    _cr3 = directory;
}

#define ENTRY_MASK (ENTRIES_PER_TABLE - 1)

// Returns the base address of the current frame
// (i.e., the address of the first byte in the frame)
static inline uint32_t _getVirtualBase(uint32_t address)
{
    return address & BASE_MASK;
}

// Returns the index in the page directory for the given address.
static inline uint32_t _getPageDirectoryIndex(uint32_t address)
{
    return address >> (OFFSET_BITS + BITS_PER_ENTRY);
}

// Returns the index in the second level page table for the given address.
static inline uint32_t _getPageTableIndex(uint32_t address)
{
    return (address >> OFFSET_BITS) & ENTRY_MASK;
}

// Returns the offset within a page / frame for the given address.
static inline uint32_t _getOffset(uint32_t address)
{
    return address & OFFSET_MASK;
}

int mapPage(uint32_t virtualBase, uint32_t physicalBase, ReadWrite accessMode,
    PrivilegeLevel privileges)
{
    if ((_getOffset(virtualBase) != 0) || (_getOffset(physicalBase) != 0)) {
        return -1;
    }

    assert(_cr3 != NULL);

    (void) accessMode;
    (void) privileges;

    // ----------------
    // Add a new page table entry.
    // If you need more space for your table data structure, you can use
    // posix_memalign() to get 4kB-Aligned blocks of memory
    // ----------------

    return 0;
}

#define TRANSLATION_ERROR 0xFFFFFFFF
// return 0xFFFFFFFF on error, physical frame number otherweise.
uint32_t translatePageTable(uint32_t virtualAddress, ReadWrite accessMode,
    PrivilegeLevel privileges)
{
    assert(_cr3 != NULL);

    (void) virtualAddress;
    (void) accessMode;
    (void) privileges;

    // ----------------
    // Implement the address translation here.
    // ----------------

    return TRANSLATION_ERROR;
}


// return -1 on error, 0 on success
int unmapPage(uint32_t virtualBase) {
    if (_getOffset(virtualBase) != 0) {
        return -1;
    }

    return -1;
}
