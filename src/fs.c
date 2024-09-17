//
// Created by antoine barbier on 17/09/2024.
//

#include "fs.h"
#ifdef _WIN32
#include <dirent.h>
#else
#include <sys/stat.h>
#endif

#include <unistd.h>
#include <errno.h>

int fs_mkdir(const char *pathname) {
#ifdef _WIN32
    // Windows-specific implementation
    if (_mkdir(pathname) == -1) {
        return -1; // Return -1 on error
    }
#else
    // POSIX implementation
    if (mkdir(pathname, 0755) == -1) {
        return -1; // Return -1 on error
    }
#endif
    return 0; // Return 0 on success
}