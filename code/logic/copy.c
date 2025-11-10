/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop high-
 * performance, cross-platform applications and libraries. The code contained
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/code/commands.h"
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <utime.h>

// Helper: copy a single file
static int copy_file(ccstring src, ccstring dest, bool update, bool preserve) {
    if (cunlikely(!cnotnull(src) || !cnotnull(dest))) {
        fossil_io_printf("{red}Error: Source and destination paths cannot be null{normal}\n");
        return 1;
    }

    struct stat st_src, st_dest;
    if (stat(src, &st_src) != 0) {
        fossil_io_printf("{red}Error: Cannot stat source file '%s': %s{normal}\n", src, strerror(errno));
        return errno;
    }

    bool dest_exists = (stat(dest, &st_dest) == 0);

    if (update && dest_exists) {
        if (st_dest.st_mtime >= st_src.st_mtime) {
            fossil_io_printf("{cyan}Skipping '%s' - destination is up to date{normal}\n", src);
            return 0;
        }
    }

    fossil_fstream_t src_stream, dest_stream;
    
    if (fossil_fstream_open(&src_stream, src, "rb") != 0) {
        fossil_io_printf("{red}Error: Cannot open source file '%s': %s{normal}\n", src, strerror(errno));
        return errno;
    }

    if (fossil_fstream_open(&dest_stream, dest, "wb") != 0) {
        fossil_io_printf("{red}Error: Cannot create destination file '%s': %s{normal}\n", dest, strerror(errno));
        fossil_fstream_close(&src_stream);
        return errno;
    }

    fossil_io_printf("{cyan}Copying file: %s -> %s{normal}\n", src, dest);

    char buffer[8192];
    size_t n;
    while ((n = fossil_fstream_read(&src_stream, buffer, 1, sizeof(buffer))) > 0) {
        if (cunlikely(fossil_fstream_write(&dest_stream, buffer, 1, n) != n)) {
            fossil_io_printf("{red}Error: Write failed for '%s': %s{normal}\n", dest, strerror(errno));
            fossil_fstream_close(&src_stream);
            fossil_fstream_close(&dest_stream);
            return errno;
        }
    }

    fossil_fstream_close(&src_stream);
    fossil_fstream_close(&dest_stream);

    // Preserve permissions and timestamps
    if (preserve) {
        chmod(dest, st_src.st_mode);
        struct utimbuf times = {st_src.st_atime, st_src.st_mtime};
        utime(dest, &times);
        fossil_io_printf("{cyan}Preserved permissions and timestamps for '%s'{normal}\n", dest);
    }

    return 0;
}

// Helper: copy directories recursively
static int copy_directory(ccstring src, ccstring dest,
                          bool recursive, bool update, bool preserve) {
    if (cunlikely(!cnotnull(src) || !cnotnull(dest))) {
        fossil_io_printf("{red}Error: Source and destination paths cannot be null{normal}\n");
        return 1;
    }

    struct stat st;
    if (stat(src, &st) != 0) { 
        fossil_io_printf("{red}Error: Cannot stat source directory '%s': %s{normal}\n", src, strerror(errno));
        return errno; 
    }

    // Create destination directory
    fossil_io_printf("{cyan}Creating directory: %s{normal}\n", dest);
    if (mkdir(dest, st.st_mode) != 0) {
        if (errno != EEXIST) { 
            fossil_io_printf("{red}Error: Cannot create directory '%s': %s{normal}\n", dest, strerror(errno));
            return errno; 
        }
    }

    DIR *dir = opendir(src);
    if (cunlikely(!cnotnull(dir))) { 
        fossil_io_printf("{red}Error: Cannot open directory '%s': %s{normal}\n", src, strerror(errno));
        return errno; 
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != cnull) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        cstring src_path = fossil_io_cstring_format("%s/%s", src, entry->d_name);
        cstring dest_path = fossil_io_cstring_format("%s/%s", dest, entry->d_name);

        if (cunlikely(!cnotnull(src_path) || !cnotnull(dest_path))) {
            fossil_io_cstring_free(src_path);
            fossil_io_cstring_free(dest_path);
            closedir(dir);
            return 1;
        }

        if (stat(src_path, &st) != 0) {
            fossil_io_cstring_free(src_path);
            fossil_io_cstring_free(dest_path);
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            if (recursive) {
                if (copy_directory(src_path, dest_path, recursive, update, preserve) != 0) {
                    fossil_io_cstring_free(src_path);
                    fossil_io_cstring_free(dest_path);
                    closedir(dir);
                    return 1;
                }
            }
        } else if (S_ISREG(st.st_mode)) {
            if (copy_file(src_path, dest_path, update, preserve) != 0) {
                fossil_io_cstring_free(src_path);
                fossil_io_cstring_free(dest_path);
                closedir(dir);
                return 1;
            }
        }
        // ignore symlinks, devices, etc. for now

        fossil_io_cstring_free(src_path);
        fossil_io_cstring_free(dest_path);
    }

    closedir(dir);

    // Preserve directory timestamps
    if (preserve) {
        struct utimbuf times = {st.st_atime, st.st_mtime};
        utime(dest, &times);
    }

    return 0;
}

/**
 * Copy files or directories with various options
 */
int fossil_shark_copy(ccstring src, ccstring dest,
                      bool recursive, bool update, bool preserve) {
    if (cunlikely(!cnotnull(src) || !cnotnull(dest))) {
        fossil_io_printf("{red}Error: Source and destination must be specified{normal}\n");
        return 1;
    }

    struct stat st;
    if (stat(src, &st) != 0) {
        fossil_io_printf("{red}Error: Cannot access source '%s': %s{normal}\n", src, strerror(errno));
        return errno;
    }

    if (S_ISDIR(st.st_mode)) {
        if (!recursive) {
            fossil_io_printf("{red}Error: Source is a directory. Use recursive flag to copy directories{normal}\n");
            return 1;
        }
        fossil_io_printf("{cyan}Starting recursive copy of directory: %s -> %s{normal}\n", src, dest);
        return copy_directory(src, dest, recursive, update, preserve);
    } else if (S_ISREG(st.st_mode)) {
        return copy_file(src, dest, update, preserve);
    } else {
        fossil_io_printf("{red}Error: Unsupported file type for '%s'{normal}\n", src);
        return 1;
    }
}
