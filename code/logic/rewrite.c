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

//
#include <time.h>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <utime.h>
#endif

/**
 * Set the access and modification times of a file.
 * Returns 0 on success, -1 on failure.
 */
int fossil_io_filesys_set_times(const char *path, time_t accessed_at, time_t modified_at)
{
    if (!path)
        return -1;

#if defined(_WIN32) || defined(_WIN64)
    // Convert time_t to FILETIME
    HANDLE hFile = CreateFileA(
        path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return -1;

    // Windows FILETIME is in 100-nanosecond intervals since Jan 1, 1601 (UTC)
    ULONGLONG unix_epoch = 11644473600ULL;
    ULONGLONG access_time = ((ULONGLONG)accessed_at + unix_epoch) * 10000000ULL;
    ULONGLONG mod_time = ((ULONGLONG)modified_at + unix_epoch) * 10000000ULL;

    FILETIME ftAccess, ftWrite;
    ftAccess.dwLowDateTime = (DWORD)access_time;
    ftAccess.dwHighDateTime = (DWORD)(access_time >> 32);
    ftWrite.dwLowDateTime = (DWORD)mod_time;
    ftWrite.dwHighDateTime = (DWORD)(mod_time >> 32);

    int result = 0;
    if (!SetFileTime(hFile, NULL, &ftAccess, &ftWrite))
        result = -1;

    CloseHandle(hFile);
    return result;
#else
    struct utimbuf times;
    times.actime = accessed_at;
    times.modtime = modified_at;
    return utime(path, &times);
#endif
}

int fossil_shark_rewrite(const char *path, bool in_place, bool append,
                         const char *new_content, size_t size,
                         bool update_access_time, bool update_mod_time)
{
    if (!cnotnull(path))
        return -1;

    fossil_io_filesys_file_t stream;
    fossil_io_filesys_file_t tmp_stream;
    char tmp_path[FOSSIL_FILESYS_MAX_PATH] = {0};

    // Use a temporary file if not modifying in place
    if (!in_place && cnotnull(new_content))
    {
        snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);
        if (fossil_io_filesys_file_open(&tmp_stream, tmp_path, "wb") != 0)
            return -1;
        size_t content_len = fossil_io_cstring_length(new_content);
        if (fossil_io_filesys_file_write(&tmp_stream, new_content, 1, content_len) != content_len)
        {
            fossil_io_filesys_file_close(&tmp_stream);
            return -1;
        }
        fossil_io_filesys_file_close(&tmp_stream);

        if (fossil_io_filesys_move(tmp_path, path) != 0)
            return -1;
    }
    else if (cnotnull(new_content))
    {
        if (fossil_io_filesys_file_open(&stream, path, append ? "ab" : "wb") != 0)
            return -1;
        size_t content_len = fossil_io_cstring_length(new_content);
        if (fossil_io_filesys_file_write(&stream, new_content, 1, content_len) != content_len)
        {
            fossil_io_filesys_file_close(&stream);
            return -1;
        }
        fossil_io_filesys_file_close(&stream);
    }

    // Handle truncating/extending file size
    if (size > 0)
    {
        if (fossil_io_filesys_file_truncate(path, size) != 0)
            return -1;
    }

    // Update file timestamps if requested
    if (update_access_time || update_mod_time)
    {
        fossil_io_filesys_obj_t obj;
        if (fossil_io_filesys_stat(path, &obj) != 0)
            return -1;
        time_t now = time(NULL);
        time_t accessed_at = obj.accessed_at;
        time_t modified_at = obj.modified_at;
        if (update_access_time)
            accessed_at = now;
        if (update_mod_time)
            modified_at = now;
        if (fossil_io_filesys_set_times(path, accessed_at, modified_at) != 0)
            return -1;
    }

    return 0;
}
