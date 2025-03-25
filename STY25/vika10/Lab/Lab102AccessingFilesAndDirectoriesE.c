// e. You can use the seek system call to create sparse files: If you seek to a position beyond
// the end of a file (and if the file system implementation supports sparse files), then the
// skipped part may remain as a ’hole’ without allocated disk storage. Verify this with a
// short sample program that creates a new file, seeks the position of 1 GiB after the start,
// writes a single byte, closes the file, and then checks the file size (total size, and block
// size on disk) using the stat system call.
// Note: This will be a second essential building lock for the assignment P10!

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>

int main() {
    // O_CREAT: create the file if it does not exist
    // O_WRONLY: open the file for writing only
    int fd = open("sparse.bin", O_CREAT | O_WRONLY, 0644); // opens or creates a file named sparse.bin with write-only permissions
    lseek(fd, 1024*1024*1024 - 1, SEEK_SET); // Seek to 1 GiB - 1 byte from the start of the file
    write(fd, "\0", 1);                      // Write 1 byte, writes a single /0 byte at the 1 GiB position
    close(fd);

    // Check file size and disk usage
    struct stat st; // File status
    stat("sparse.bin", &st); // Get file status
    printf("Size: %ld bytes\n", st.st_size);       // 1073741825 bytes
    printf("Blocks: %ld (512B each)\n", st.st_blocks); // 8 blocks 
    return 0;
}

// Compile the program using the following command:
// gcc Lab102AccessingFilesAndDirectoriesE.c -o Lab102AccessingFilesAndDirectoriesE
// Run the program using the following command:
// ./Lab102AccessingFilesAndDirectoriesE
// Output:
// File size: 1073741825 bytes
// Block size: 4096 bytes