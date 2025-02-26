#pragma GCC diagnostic ignored "-Wunused-function"

#define _POSIX_C_SOURCE 200112L
#include "page_table.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

    // ----------------
    // Add a new page table entry.
    // If you need more space for your table data structure, you can use
    // posix_memalign() to get 4kB-Aligned blocks of memory
    // ----------------
    // Use cr3 to get the page directory and find the correct page directory entry.

    uint32_t pageDirectoryIndex = _getPageDirectoryIndex(virtualBase);
    uint32_t pageTableIndex = _getPageTableIndex(virtualBase);
    
    uint64_t pde = _cr3->entries[pageDirectoryIndex];
    PageTable *pageTable = NULL;
    if (!(pde & PAGE_PRESENT_MASK)) {
        // • If the PDE is not valid: Allocate a new page table page using posix memalign() to
        // ensure page-aligned memory (with the low 12 bits zero). Use pointerToInt() to
        // convert the resulting pointer to an integer. Initialize the entire new page table page
        // to zero. Do not forget to set the present bit in the PDE when updating the page
        // directory.
        // PDE is not valid, so we need to allocate a new page table
        if (posix_memalign((void**)&pageTable, 4096, sizeof(PageTable)) != 0) {
            return -1; // page table allocation failed
        }
        memset(pageTable, 0, sizeof(PageTable)); // initialize the page table to zero
        // Update the PDE, store the new page table's address with the present bit set
        _cr3->entries[pageDirectoryIndex] = pointerToInt(pageTable) | PAGE_PRESENT_MASK; 
    }
    else {
        // retreive the existing page table from the PDE
        pageTable = intToPointer(pde & PAGE_DIRECTORY_ADDRESS_MASK);
    }
    // • Build the new PTE with the desired access rights and update the corresponding
    // entry in the page table.
    // BUILD
    uint32_t PTE = physicalBase | PAGE_PRESENT_MASK;
    if (accessMode == ACCESS_WRITE) {
        PTE |= PAGE_READWRITE_MASK;
    }
    if (privileges == USER_MODE) {
        PTE |= PAGE_USERMODE_MASK;
    }
    // UPDATE
    pageTable->entries[pageTableIndex] = PTE;

    // • Returns 0 on success, -1 otherwise (e.g., page table allocation failed).
    return 0;
}

#define TRANSLATION_ERROR 0xFFFFFFFF
// return 0xFFFFFFFF on error, physical frame number otherweise.
uint32_t translatePageTable(uint32_t virtualAddress, ReadWrite accessMode,
    PrivilegeLevel privileges)
{
    assert(_cr3 != NULL);
    // ----------------
    // Implement the address translation here.
    // ----------------
    // uint32_t _virtualBase = _getVirtualBase(virtualAddress);
    uint32_t pageDirectoryIndex = _getPageDirectoryIndex(virtualAddress);
    uint32_t pageTableIndex = _getPageTableIndex(virtualAddress);
    uint32_t offset = _getOffset(virtualAddress);
    // printf("Page Directory Index: %d\n", pageDirectoryIndex);
    // printf("Page Table Index: %d\n", pageTableIndex);
    // printf("Offset: %d\n", offset);
    
    // now we have the page directory index and the page table index
    // we can use that to locate the page table page
    uint64_t pde = _cr3->entries[pageDirectoryIndex]; // get the PDE corresponding to the virtual address
    if (!(pde & PAGE_PRESENT_MASK)) { // is the virtual address present in the page directory?
        return INVALID_ADDRESS; // page table not present
    }
    
    // extract the page table's base address from the PDE
    PageTable *pageTable = intToPointer(pde & PAGE_DIRECTORY_ADDRESS_MASK);
    // check if it correctly converted the pointer
    // now we can get the PTE corresponding to the virtual address (second level)
    uint32_t PTE = pageTable->entries[pageTableIndex]; 
    if (!(PTE & PAGE_PRESENT_MASK)) { // is the virtual address present in the page table?
        return INVALID_ADDRESS;
    }
    
    // PERMISSION CHECKS
    // checking if the requested access mode is permitted or if the privilege level is insufficient
    // check for write access
    if (privileges == USER_MODE) {
        if (accessMode == ACCESS_WRITE && !(PTE & PAGE_READWRITE_MASK)) {
            return INVALID_ADDRESS;
        }
        if (!(PTE & PAGE_USERMODE_MASK)) {
            return INVALID_ADDRESS;
        }
    }
    // check for privilege level / user mode access

    // mark the PTE as accessed by updating its accessed flag
    pageTable->entries[pageTableIndex] |= PAGE_ACCESSED_MASK;
    
    // if all checks pass, perform the translation to compute the physical address
    // we need to extract the physical frame number from the PTE and add the offset
    uint32_t frame_base = PTE & PAGE_TABLE_ADDRESS_MASK;
    uint32_t physicalAddress = frame_base + offset;

    return physicalAddress;
}


// return -1 on error, 0 on success
// Implement a function that unmaps a virtual page by marking its PTE as invalid.
int unmapPage(uint32_t virtualBase) {
    if (_getOffset(virtualBase) != 0) {
        return -1;
    }
    assert(_cr3 != NULL);

    uint32_t PageDirectoryIndex = _getPageDirectoryIndex(virtualBase);
    uint32_t PageTableIndex = _getPageTableIndex(virtualBase);

    uint64_t pde = _cr3->entries[PageDirectoryIndex];
    if (!(pde & PAGE_PRESENT_MASK)) {
        return -1; // page table not present for the virtual address
    }
    // Get the page table.
    PageTable* pageTable = (PageTable*) intToPointer(pde & PAGE_DIRECTORY_ADDRESS_MASK);
    uint32_t PTE = pageTable->entries[PageTableIndex];
    if (!(PTE & PAGE_PRESENT_MASK)) {
        // The page is already unmapped.
        return -1;
    }

    // • Mark the page as invalid by clearing the present bit in its PTE.
    pageTable->entries[PageTableIndex] &= ~PAGE_PRESENT_MASK;

    // Optionally, check if the entire page table is now empty.
    int all_unmapped = 1;
    for (int i = 0; i < ENTRIES_PER_TABLE; i++) {
        if (pageTable->entries[i] & PAGE_PRESENT_MASK) {
            all_unmapped = 0;
            break;
        }
    }
    // • If all entries in the PTE are marked as not present, you may deallocate the page
    // table page (using free() and mark the entry in the page directory (PDE) as not
    // present.
    if (all_unmapped) {
        free(pageTable);
        _cr3->entries[PageDirectoryIndex] &= ~PAGE_PRESENT_MASK;
    }

    return 0;
}
