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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <utime.h>
#endif

int fossil_shark_rewrite(const char *path, bool in_place, bool append,
                         const char *new_content, size_t size,
                         bool update_access_time, bool update_mod_time) 
{
    if (!path) return -1;

    FILE *fp = NULL;

    // Handle content writing
    if (new_content) {
        fp = fopen(path, append ? "ab" : "wb");
        if (!fp) return errno;
        if (fwrite(new_content, 1, strlen(new_content), fp) != strlen(new_content)) {
            fclose(fp);
            return errno;
        }
        fclose(fp);
    }

    // Handle truncating/extending file size
    if (size > 0) {
#if defined(_WIN32) || defined(_WIN64)
        HANDLE hFile = CreateFileA(path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE) return -1;
        LARGE_INTEGER li;
        li.QuadPart = size;
        if (SetFilePointerEx(hFile, li, NULL, FILE_BEGIN) == 0) {
            CloseHandle(hFile);
            return -1;
        }
        if (SetEndOfFile(hFile) == 0) {
            CloseHandle(hFile);
            return -1;
        }
        CloseHandle(hFile);
#else
        if (truncate(path, size) != 0) return errno;
#endif
    }

    // Update file timestamps if requested
    if (update_access_time || update_mod_time) {
#if defined(_WIN32) || defined(_WIN64)
        HANDLE hFile = CreateFileA(path, FILE_WRITE_ATTRIBUTES, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE) return -1;
        FILETIME ftCreate, ftAccess, ftWrite;
        SYSTEMTIME stUTC;
        GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
        GetSystemTime(&stUTC);
        SystemTimeToFileTime(&stUTC, &ftWrite);
        if (!SetFileTime(hFile,
                         update_access_time ? NULL : &ftCreate,
                         update_access_time ? &ftWrite : &ftAccess,
                         update_mod_time ? &ftWrite : &ftWrite)) {
            CloseHandle(hFile);
            return -1;
        }
        CloseHandle(hFile);
#else
        struct stat st;
        if (stat(path, &st) != 0) return errno;
        struct utimbuf new_times = {
            .actime = update_access_time ? time(NULL) : st.st_atime,
            .modtime = update_mod_time ? time(NULL) : st.st_mtime
        };
        if (utime(path, &new_times) != 0) return errno;
#endif
    }

    return 0;
}
