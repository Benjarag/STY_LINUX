#ifndef COPY_H
#define COPY_H

typedef struct {
	const char* src;
	const char* dst;

	int blocksize;		   // Number of bytes to copy per system call (read/write)
} CopyArgs;

int parseCopyArgs(int argc, char * const argv[], CopyArgs* args);

int doCopy(CopyArgs* args);

#endif
