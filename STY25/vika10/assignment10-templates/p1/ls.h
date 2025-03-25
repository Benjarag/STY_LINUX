#ifndef LS_H
#define LS_H

#define _POSIX_C_SOURCE 200809L

//#pragma GCC diagnostic ignored "-Wformat-truncation"

#include <stdio.h>
#include <stddef.h>

// Use this function to display the files. DO NOT CHANGE IT.
void printLine(size_t size, const char *mode, const char* name, const char *typestr);


// Assume this to be the maximum length of a file name returned by readdir
#define MAX_FILE_NAME_LENGTH 255


// This is the function to implement...
int list(const char* path, int recursive);

#endif

