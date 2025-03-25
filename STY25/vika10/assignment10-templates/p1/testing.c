

#include "ls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

static void get_mode_string(struct stat *st, char *mode_str) {
    mode_str[0] = '-';
    if (S_ISDIR(st->st_mode)) {
        mode_str[0] = 'd';
    } else if (S_ISLNK(st->st_mode)) {
        mode_str[0] = 'l';
    } else if (S_ISFIFO(st->st_mode)) {
        mode_str[0] = 'p';
    }

    mode_str[1] = (st->st_mode & S_IRUSR) ? 'r' : '-';
    mode_str[2] = (st->st_mode & S_IWUSR) ? 'w' : '-';
    mode_str[3] = (st->st_mode & S_IXUSR) ? 'x' : '-';

    mode_str[4] = (st->st_mode & S_IRGRP) ? 'r' : '-';
    mode_str[5] = (st->st_mode & S_IWGRP) ? 'w' : '-';
    mode_str[6] = (st->st_mode & S_IXGRP) ? 'x' : '-';

    mode_str[7] = (st->st_mode & S_IROTH) ? 'r' : '-';
    mode_str[8] = (st->st_mode & S_IWOTH) ? 'w' : '-';
    mode_str[9] = (st->st_mode & S_IXOTH) ? 'x' : '-';
    mode_str[10] = '\0';
}

static int compare_names(const void *a, const void *b) {
    const char *nameA = *(const char **)a;
    const char *nameB = *(const char **)b;
    return strcmp(nameA, nameB);
}

int list(const char* path, int recursive) {
    DIR *dir = opendir(path);
    if (!dir) {
        return -1;
    }

    char **names = NULL;
    size_t count = 0;
    struct dirent *entry;

    // Read all directory entries into 'names' array
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        names = realloc(names, (count + 1) * sizeof(char *));
        if (!names) {
            closedir(dir);
            return -1;
        }
        names[count] = strdup(entry->d_name);
        if (!names[count]) {
            closedir(dir);
            free(names);
            return -1;
        }
        count++;
    }
    closedir(dir);

    // Sort the names using byte-wise comparison
    qsort(names, count, sizeof(char *), compare_names);

    // Process each sorted entry
    for (size_t i = 0; i < count; i++) {
        const char *name = names[i];
        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);

        struct stat st;
        if (lstat(fullpath, &st) == -1) {
            free(names[i]);
            continue;
        }

        char mode_str[11];
        get_mode_string(&st, mode_str);

        char typestr[1024] = "";
        if (S_ISDIR(st.st_mode)) {
            strcpy(typestr, "/");
        } else if (S_ISFIFO(st.st_mode)) {
            strcpy(typestr, "|");
        } else if (S_ISLNK(st.st_mode)) {
            char target[PATH_MAX];
            ssize_t len = readlink(fullpath, target, sizeof(target) - 1);
            if (len != -1) {
                target[len] = '\0';
                snprintf(typestr, sizeof(typestr), " -> %.1000s", target);
            }
        } else if (S_ISREG(st.st_mode) && (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
            strcpy(typestr, "*");
        }

        printLine((size_t)st.st_size, mode_str, fullpath, typestr);

        if (recursive && S_ISDIR(st.st_mode) && !S_ISLNK(st.st_mode)) {
            list(fullpath, recursive);
        }

        free(names[i]);
    }

    free(names);
    return 0;
}