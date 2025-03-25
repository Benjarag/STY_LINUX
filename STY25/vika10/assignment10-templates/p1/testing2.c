/* 
* Group number (on canvas): xx
* Student 1 name: Benjamín Ragnarsson 
* Student 2 name: xx 
*/

#include "ls.h"

// You may not need all these headers ...
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> // Required for AT_SYMLINK_NOFOLLOW
#include <limits.h> // Required for PATH_MAX

int list(const char* path, int recursive)
{
    // first we open the directory
    DIR *dir = opendir(path);
    if (!dir) {
        return -1;
    }
    struct dirent *entry; // directory entry
    
    // read all entries in the directory one by one
    while ((entry = readdir(dir)) != NULL) {
        // skip the current and parent directory
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct full path: if path is "p1" and entry is "ls.c", result is "p1/ls.c"
        char *name_path = malloc(strlen(path) + strlen(entry->d_name) + 2); // +2 for '/' and '\0'
        if (!name_path) {
            closedir(dir);
            return -1;
        }
        strcpy(name_path, path);
        strcat(name_path, "/");
        strcat(name_path, entry->d_name);

        // Get the file information using lstat (to not follow symlinks)
        struct stat st;
        if (lstat(name_path, &st) == -1) {
            free(name_path);
            continue;
        }
        
        // Get the file type and prepare mode string and typestr
        mode_t filetype = st.st_mode & S_IFMT;
        
        char mode[11];
        char *typestr = "";  
        char buffer[1024] = {0};
        
        if (filetype == S_IFIFO) {
            mode[0] = 'p';
            typestr = "|";
        } else if (filetype == S_IFDIR) {
            mode[0] = 'd';
            typestr = "/";
        } else if (filetype == S_IFLNK) {
            mode[0] = 'l';
            typestr = buffer;
            strcpy(buffer, " -> ");
            // Use full path when reading the symbolic link target
            ssize_t len = readlink(name_path, buffer + 4, 1020);
            if (len == -1) {
                // If error, leave buffer as " -> "
                buffer[4] = '\0';
            } else {
                buffer[len + 4] = '\0';
            }
        } else if (filetype == S_IFREG && (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
            mode[0] = '-';
            typestr = "*";
        } else {
            mode[0] = '-';
        }

        // Owner permissions
        mode[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
        mode[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
        mode[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';
        // Group permissions
        mode[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
        mode[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
        mode[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';
        // Others permissions
        mode[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
        mode[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
        mode[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';
        mode[10] = '\0'; // Null terminator
    
        printLine((size_t)st.st_size, mode, name_path, typestr);
        
        // Recursively descend into subdirectories if recursive is nonzero.
        if (recursive && filetype == S_IFDIR) {
            // Do not follow symlink directories
            list(name_path, recursive);
        }
        free(name_path);
    }
    closedir(dir);

    return 0;
}
