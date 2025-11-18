/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2014
 *
 * Copyright (C) 2014-2025 Fossil Logic. All rights reserved.
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
    if (!cnotnull(path)) return -1;

    fossil_fstream_t stream;
    fossil_fstream_t tmp_stream;
    fossil_sys_memory_t tmp_path = fossil_sys_memory_calloc(1, FILENAME_MAX);

    // Use a temporary file if not modifying in place
    if (!in_place && cnotnull(new_content)) {
        snprintf((char *)tmp_path, FILENAME_MAX, "%s.tmp", path);
        if (fossil_fstream_open(&tmp_stream, (char *)tmp_path, "wb") != 0) {
            fossil_sys_memory_free(tmp_path);
            return errno;
        }
        size_t content_len = fossil_io_cstring_length(new_content);
        if (fossil_fstream_write(&tmp_stream, new_content, 1, content_len) != content_len) {
            fossil_fstream_close(&tmp_stream);
            fossil_sys_memory_free(tmp_path);
            return errno;
        }
        fossil_fstream_close(&tmp_stream);

#if defined(_WIN32) || defined(_WIN64)
        if (fossil_fstream_rename((char *)tmp_path, path) != 0) {
            fossil_sys_memory_free(tmp_path);
            return errno;
        }
#else
        if (fossil_fstream_rename((char *)tmp_path, path) != 0) {
            fossil_sys_memory_free(tmp_path);
            return errno;
        }
#endif
    } else if (cnotnull(new_content)) {
        if (fossil_fstream_open(&stream, path, append ? "ab" : "wb") != 0) {
            fossil_sys_memory_free(tmp_path);
            return errno;
        }
        size_t content_len = fossil_io_cstring_length(new_content);
        if (fossil_fstream_write(&stream, new_content, 1, content_len) != content_len) {
            fossil_fstream_close(&stream);
            fossil_sys_memory_free(tmp_path);
            return errno;
        }
        fossil_fstream_close(&stream);
    }

    fossil_sys_memory_free(tmp_path);

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
        int fd = open(path, O_WRONLY);
        if (fd < 0) return errno;
        int rc = ftruncate(fd, (off_t)size);
        int saved = errno;
        close(fd);
        if (rc != 0) return saved;
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
