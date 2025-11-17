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

/**
 * Modify or update file contents, timestamps, or size
 * @param path Path to the file to modify
 * @param in_place If true, modify the file directly; if false, write via temporary file
 * @param append Append content instead of overwriting
 * @param new_content Content to write to the file (NULL if only updating timestamps/size)
 * @param size Set exact file size (truncate or extend)
 * @param update_access_time Update file access time
 * @param update_mod_time Update file modification time
 * @return 0 on success, non-zero on error
 */
int fossil_shark_rewrite(const char *path, bool in_place, bool append,
                         const char *new_content, size_t size,
                         bool update_access_time, bool update_mod_time)
{
    if (!path) return -1;

    // Use a temporary file if not modifying in place
    char tmp_path[FILENAME_MAX];
    FILE *fp = NULL;
    if (!in_place && new_content) {
        snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);
        fp = fopen(tmp_path, "wb");
    } else if (new_content) {
        fp = fopen(path, append ? "ab" : "wb");
    }

    if (new_content) {
        if (!fp) return errno;
        if (fwrite(new_content, 1, strlen(new_content), fp) != strlen(new_content)) {
            fclose(fp);
            return errno;
        }
        fclose(fp);

        if (!in_place) {
            // Replace original file with temporary file
#if defined(_WIN32) || defined(_WIN64)
            if (!MoveFileExA(tmp_path, path, MOVEFILE_REPLACE_EXISTING)) return -1;
#else
            if (rename(tmp_path, path) != 0) return errno;
#endif
        }
    }

    // Handle truncating/extending file size
    if (size > 0) {
#if defined(_WIN32) || defined(_WIN64)
        HANDLE hFile = CreateFileA(path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE) return -1;
        LARGE_INTEGER li;
        li.QuadPart = size;
        if (SetFilePointerEx(hFile, li, NULL, FILE_BEGIN) == 0 || SetEndOfFile(hFile) == 0) {
            CloseHandle(hFile);
            return -1;
        }
        CloseHandle(hFile);
#else
        {
            int fd = open(path, O_WRONLY);
            if (fd < 0) return errno;
        
            int rc = ftruncate(fd, (off_t)size);
            int saved = errno;
        
            close(fd);
            if (rc != 0) return saved;
        }
#endif
    }

    // Update file timestamps if requested
    if (update_access_time || update_mod_time) {
#if defined(_WIN32) || defined(_WIN64)
        HANDLE hFile = CreateFileA(path, FILE_WRITE_ATTRIBUTES, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE) return -1;
        FILETIME ftCreate, ftAccess, ftWrite;
        GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
        SYSTEMTIME stUTC;
        GetSystemTime(&stUTC);
        SystemTimeToFileTime(&stUTC, &ftWrite);
        if (!SetFileTime(hFile,
                         NULL,
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
