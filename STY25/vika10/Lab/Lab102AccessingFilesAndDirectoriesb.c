// b. What system calls do you need to list the files in a directory in Linux? Implement a
// simple function that prints all file names in the current directory!
// Note: This will be a essential building lock for the assignment P10!

#include <dirent.h>
#include <stdio.h>

void list_files() {
    DIR *dir = opendir("."); // Open the current directory
    struct dirent *entry; // Pointer to a directory entry

    while ((entry = readdir(dir)) != NULL) { // Read each file in the directory
        if (entry->d_name[0] != '.') { // Skip hidden files (e.g., ".", "..", ".ssh")
            printf("%s\n", entry->d_name); // Print the file name
        }
    }
    closedir(dir); // Close the directory
}

int main() {
    list_files(); // Call the function to list files in the current directory
    return 0;
}
