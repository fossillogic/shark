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

#ifdef _WIN32
#include <windows.h>
#endif

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
#ifdef _WIN32
        // Windows: Set file attributes and times
        HANDLE hFile = CreateFileA(dest, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            FILETIME ft;
            // Convert Unix timestamp to Windows FILETIME
            LONGLONG ll = Int32x32To64(st_src.st_mtime, 10000000) + 116444736000000000;
            ft.dwLowDateTime = (DWORD)ll;
            ft.dwHighDateTime = (DWORD)(ll >> 32);
            SetFileTime(hFile, NULL, NULL, &ft);
            CloseHandle(hFile);
        }
#else
        chmod(dest, st_src.st_mode);
        struct utimbuf times = {st_src.st_atime, st_src.st_mtime};
        utime(dest, &times);
#endif
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
#ifdef _WIN32
    if (CreateDirectoryA(dest, NULL) == 0) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            fossil_io_printf("{red}Error: Cannot create directory '%s': Error %lu{normal}\n", dest, GetLastError());
            return 1;
        }
    }
#else
    if (mkdir(dest, st.st_mode) != 0) {
        if (errno != EEXIST) { 
            fossil_io_printf("{red}Error: Cannot create directory '%s': %s{normal}\n", dest, strerror(errno));
            return errno; 
        }
    }
#endif

#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*", src);
    
    HANDLE hFind = FindFirstFileA(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        fossil_io_printf("{red}Error: Cannot open directory '%s': Error %lu{normal}\n", src, GetLastError());
        return 1;
    }

    do {
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;

        cstring src_path = fossil_io_cstring_format("%s\\%s", src, findData.cFileName);
        cstring dest_path = fossil_io_cstring_format("%s\\%s", dest, findData.cFileName);
#else
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
#endif

        if (cunlikely(!cnotnull(src_path) || !cnotnull(dest_path))) {
            fossil_io_cstring_free(src_path);
            fossil_io_cstring_free(dest_path);
#ifdef _WIN32
            FindClose(hFind);
#else
            closedir(dir);
#endif
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
#ifdef _WIN32
                    FindClose(hFind);
#else
                    closedir(dir);
#endif
                    return 1;
                }
            }
        } else if (S_ISREG(st.st_mode)) {
            if (copy_file(src_path, dest_path, update, preserve) != 0) {
                fossil_io_cstring_free(src_path);
                fossil_io_cstring_free(dest_path);
#ifdef _WIN32
                FindClose(hFind);
#else
                closedir(dir);
#endif
                return 1;
            }
        }

        fossil_io_cstring_free(src_path);
        fossil_io_cstring_free(dest_path);
#ifdef _WIN32
    } while (FindNextFileA(hFind, &findData) != 0);
    FindClose(hFind);
#else
    }
    closedir(dir);
#endif

    // Preserve directory timestamps
    if (preserve) {
#ifdef _WIN32
        HANDLE hDir = CreateFileA(dest, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, 
                                  OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
        if (hDir != INVALID_HANDLE_VALUE) {
            FILETIME ft;
            LONGLONG ll = Int32x32To64(st.st_mtime, 10000000) + 116444736000000000;
            ft.dwLowDateTime = (DWORD)ll;
            ft.dwHighDateTime = (DWORD)(ll >> 32);
            SetFileTime(hDir, NULL, NULL, &ft);
            CloseHandle(hDir);
        }
#else
        struct utimbuf times = {st.st_atime, st.st_mtime};
        utime(dest, &times);
#endif
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
