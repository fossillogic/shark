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


__attribute__((unused))
static void fossil_shark_watch_file(const char *path, const char *events, struct stat *prev_stat)
{
    struct stat curr_stat;
    int rc = stat(path, &curr_stat);
    if (rc != 0) {
        if (errno == ENOENT && fossil_io_cstring_icontains(events, "delete")) {
            cstring del_msg = fossil_io_cstring_format("{red}File deleted:{normal} %s\n", path);
            fossil_io_file_write(FOSSIL_STDOUT, del_msg, fossil_io_cstring_length(del_msg), 1);
            fossil_io_cstring_free(del_msg);
        }
        return;
    }

    // Check modification time
    if (curr_stat.st_mtime != prev_stat->st_mtime && fossil_io_cstring_icontains(events, "modify")) {
        cstring mod_msg = fossil_io_cstring_format("{yellow}File modified:{normal} %s\n", path);
        fossil_io_file_write(FOSSIL_STDOUT, mod_msg, fossil_io_cstring_length(mod_msg), 1);
        fossil_io_cstring_free(mod_msg);
    }

    // Check size change
    if (curr_stat.st_size != prev_stat->st_size && fossil_io_cstring_icontains(events, "modify")) {
        cstring size_msg = fossil_io_cstring_format("{cyan}File size changed:{normal} %s\n", path);
        fossil_io_file_write(FOSSIL_STDOUT, size_msg, fossil_io_cstring_length(size_msg), 1);
        fossil_io_cstring_free(size_msg);
    }

    *prev_stat = curr_stat;
}

#if !defined(_WIN32) && !defined(_WIN64)
__attribute__((unused))
static void fossil_shark_watch_dir(const char *dir_path, const char *events, int interval)
{
    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, dir_path) != 0)
        return;

    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *entry = &it.current;
        if (strcmp(entry->name, ".") == 0 || strcmp(entry->name, "..") == 0)
            continue;

        if (entry->type == 1) { // Directory
            fossil_shark_watch_dir(entry->path, events, interval);
        } else if (entry->type == 0) { // File
            struct stat st;
            if (stat(entry->path, &st) == 0) {
                struct stat prev_stat = st;
                while (1) {
                    sleep(interval);
                    fossil_shark_watch_file(entry->path, events, &prev_stat);
                }
            }
        }
    }
    fossil_io_dir_iter_close(&it);
}
#endif

int fossil_shark_watch(const char *path, bool recursive,
                       const char *events, int interval)
{
#if defined(_WIN32) || defined(_WIN64)
    // Recursive not implemented for Windows in this example
    WIN32_FILE_ATTRIBUTE_DATA prev_attr;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &prev_attr)) {
        cstring err_msg = fossil_io_cstring_format("{red,bold}Failed to stat path:{normal} %s\n", path);
        fossil_io_file_write(FOSSIL_STDERR, err_msg, fossil_io_cstring_length(err_msg), 1);
        fossil_io_cstring_free(err_msg);
        return GetLastError();
    }

    cstring msg = fossil_io_cstring_format("{green,bold}Watching %s every %d seconds...{normal}%s\n", path, interval, recursive ? " (recursive not implemented on Windows)" : "");
    fossil_io_file_write(FOSSIL_STDOUT, msg, fossil_io_cstring_length(msg), 1);
    fossil_io_cstring_free(msg);

    while (1) {
        Sleep(interval * 1000);

        WIN32_FILE_ATTRIBUTE_DATA curr_attr;
        if (!GetFileAttributesExA(path, GetFileExInfoStandard, &curr_attr)) {
            DWORD err = GetLastError();
            if (err == ERROR_FILE_NOT_FOUND && fossil_io_cstring_icontains(events, "delete")) {
                cstring del_msg = fossil_io_cstring_format("{red}File deleted:{normal} %s\n", path);
                fossil_io_file_write(FOSSIL_STDOUT, del_msg, fossil_io_cstring_length(del_msg), 1);
                fossil_io_cstring_free(del_msg);
            }
            continue;
        }

        // Check modification time
        if (curr_attr.ftLastWriteTime.dwLowDateTime != prev_attr.ftLastWriteTime.dwLowDateTime ||
            curr_attr.ftLastWriteTime.dwHighDateTime != prev_attr.ftLastWriteTime.dwHighDateTime) {
            if (fossil_io_cstring_icontains(events, "modify")) {
                cstring mod_msg = fossil_io_cstring_format("{yellow}File modified:{normal} %s\n", path);
                fossil_io_file_write(FOSSIL_STDOUT, mod_msg, fossil_io_cstring_length(mod_msg), 1);
                fossil_io_cstring_free(mod_msg);
            }
        }

        // Check size change
        ULONGLONG prev_size = ((ULONGLONG)prev_attr.nFileSizeHigh << 32) | prev_attr.nFileSizeLow;
        ULONGLONG curr_size = ((ULONGLONG)curr_attr.nFileSizeHigh << 32) | curr_attr.nFileSizeLow;
        if (curr_size != prev_size && fossil_io_cstring_icontains(events, "modify")) {
            cstring size_msg = fossil_io_cstring_format("{cyan}File size changed:{normal} %s\n", path);
            fossil_io_file_write(FOSSIL_STDOUT, size_msg, fossil_io_cstring_length(size_msg), 1);
            fossil_io_cstring_free(size_msg);
        }

        prev_attr = curr_attr;
    }
#else
    struct stat st;
    if (stat(path, &st) != 0) {
        cstring err_msg = fossil_io_cstring_format("{red,bold}Failed to stat path:{normal} %s\n", path);
        fossil_io_file_write(FOSSIL_STDERR, err_msg, fossil_io_cstring_length(err_msg), 1);
        fossil_io_cstring_free(err_msg);
        return errno;
    }

    cstring msg = fossil_io_cstring_format("{green,bold}Watching %s every %d seconds...{normal}%s\n", path, interval, recursive ? " (recursive enabled)" : "");
    fossil_io_file_write(FOSSIL_STDOUT, msg, fossil_io_cstring_length(msg), 1);
    fossil_io_cstring_free(msg);

    if (recursive && S_ISDIR(st.st_mode)) {
        fossil_shark_watch_dir(path, events, interval);
    } else {
        struct stat prev_stat = st;
        while (1) {
            sleep(interval);
            fossil_shark_watch_file(path, events, &prev_stat);
        }
    }
#endif

    return 0;
}
