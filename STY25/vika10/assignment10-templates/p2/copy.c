/* 
 * Group number (on canvas): xx
 * Student 1 name: xx 
 * Student 2 name: xx 
 */

// #define _POSIX_C_SOURCE 2
#define _POSIX_C_SOURCE 200809L
#include "copy.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

// No need to change this. Parses argc into the CopyArgs structure
int parseCopyArgs(int argc, char * const argv[], CopyArgs* args)
{
	if (args == NULL) {
		return -1;
	}

	// Initialize with default values
	args->blocksize  = 4096;

	int opt;
	while ((opt = getopt(argc, argv, "b:")) != -1) {
		switch (opt) {
			case 'b':
				args->blocksize = atoi(optarg);

				if ((errno != 0) || (args->blocksize <= 0)) {
					return -1; // Invalid blocksize argument.
				}

				break;

			default: /* '?' */
				return -1;
		}
	}

	if (optind != argc - 2) {
		return -1; // We expect two parameters after the options.
	}

	args->src = argv[optind];
	args->dst = argv[optind + 1];

	return 0;
}


int doCopy(CopyArgs* args)
{
	if (args == NULL) {
		return -1;
	}

	// ----------------
	// Copy the file.
	// ----------------

	// Open source file for reading
	int src_fd = open(args->src, O_RDONLY); 
	if (src_fd < 0) {
		perror("Error opening source file");
		return -1;
	}

	// Get source file permissions
	struct stat src_stat;
    if (fstat(src_fd, &src_stat) < 0) {
        perror("Error getting source file stats");
        close(src_fd);
        return -1;
    }

	// Open destination file - O_EXCL ensures we fail if file exists
	int dest_fd = open(args->dst, O_WRONLY | O_CREAT | O_EXCL, 0644);
	if (dest_fd < 0) {
		perror("Error opening destination file");
		close(src_fd);
		return -1;
	}

	// Allocate buffer of the specified blocksize
	char * buffer = malloc(args->blocksize);
	if (!buffer) {
		perror("Memory allocation failed");
		close(src_fd);
		close(dest_fd);
		return -1;
	}

	ssize_t bytes_read, bytes_written;
	int result = 0;

	// Read and write in blocks until EOF
	while ((bytes_read = read(src_fd, buffer, args->blocksize)) > 0) {
		// Check if the block consists only of zeros
		int all_zero_bytes = 1;
		for (ssize_t i = 0; i < bytes_read; i++) {
			if (buffer[i] != 0) { // if any byte is non zero
				all_zero_bytes = 0; // this is not true
				break; // and we break 
			}
		}
		if (all_zero_bytes) {
			// For sparse files, we need to advance the file pointer
			// without writing any data - this creates a "hole"
			off_t current_pos = lseek(dest_fd, 0, SEEK_CUR); // get the current position
			if (current_pos < 0 || lseek(dest_fd, bytes_read, SEEK_CUR) < 0) {
				perror("lseek error");
				result = -1;
				goto cleanup;
			}
		}
		else {
			// Regular non-zero data - write it as usual
			bytes_written = write(dest_fd, buffer, bytes_read);
			if (bytes_written < 0) {
				perror("Write error");
				result = -1;
				goto cleanup;
			}
		}
	}

	// Check for read error
	if (bytes_read < 0) {
		perror("Read error");
		result = -1;
	}

    // Set the same permissions as the source file
    if (result == 0) {
        if (fchmod(dest_fd, src_stat.st_mode) < 0) {
            perror("Error setting file permissions");
            // Not failing the whole operation just because of permissions
        }
    }
cleanup:
	// Clean up resources
	free(buffer);
	close(src_fd);
	close(dest_fd);
	
	return result;
}
