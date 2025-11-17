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
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

int fossil_shark_watch(const char *path, bool recursive,
                       const char *events, int interval)
{
#if defined(_WIN32) || defined(_WIN64)
    WIN32_FILE_ATTRIBUTE_DATA prev_attr;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &prev_attr)) {
        cstring err_msg = fossil_io_cstring_format("{red,bold}Failed to stat path:{normal} %s\n", path);
        fossil_fstream_write(FOSSIL_STDERR, err_msg, fossil_io_cstring_length(err_msg), 1);
        fossil_io_cstring_free(err_msg);
        return GetLastError();
    }

    cstring msg = fossil_io_cstring_format("{green,bold}Watching %s every %d seconds...{normal}\n", path, interval);
    fossil_fstream_write(FOSSIL_STDOUT, msg, fossil_io_cstring_length(msg), 1);
    fossil_io_cstring_free(msg);

    while (1) {
        Sleep(interval * 1000);

        WIN32_FILE_ATTRIBUTE_DATA curr_attr;
        if (!GetFileAttributesExA(path, GetFileExInfoStandard, &curr_attr)) {
            DWORD err = GetLastError();
            if (err == ERROR_FILE_NOT_FOUND && fossil_io_cstring_icontains(events, "delete")) {
                cstring del_msg = fossil_io_cstring_format("{red}File deleted:{normal} %s\n", path);
                fossil_fstream_write(FOSSIL_STDOUT, del_msg, fossil_io_cstring_length(del_msg), 1);
                fossil_io_cstring_free(del_msg);
            }
            continue;
        }

        // Check modification time
        if (curr_attr.ftLastWriteTime.dwLowDateTime != prev_attr.ftLastWriteTime.dwLowDateTime ||
            curr_attr.ftLastWriteTime.dwHighDateTime != prev_attr.ftLastWriteTime.dwHighDateTime) {
            if (fossil_io_cstring_icontains(events, "modify")) {
                cstring mod_msg = fossil_io_cstring_format("{yellow}File modified:{normal} %s\n", path);
                fossil_fstream_write(FOSSIL_STDOUT, mod_msg, fossil_io_cstring_length(mod_msg), 1);
                fossil_io_cstring_free(mod_msg);
            }
        }

        // Check size change
        ULONGLONG prev_size = ((ULONGLONG)prev_attr.nFileSizeHigh << 32) | prev_attr.nFileSizeLow;
        ULONGLONG curr_size = ((ULONGLONG)curr_attr.nFileSizeHigh << 32) | curr_attr.nFileSizeLow;
        if (curr_size != prev_size && fossil_io_cstring_icontains(events, "modify")) {
            cstring size_msg = fossil_io_cstring_format("{cyan}File size changed:{normal} %s\n", path);
            fossil_fstream_write(FOSSIL_STDOUT, size_msg, fossil_io_cstring_length(size_msg), 1);
            fossil_io_cstring_free(size_msg);
        }

        prev_attr = curr_attr;
    }
#else
    struct stat prev_stat;
    if (stat(path, &prev_stat) != 0) {
        cstring err_msg = fossil_io_cstring_format("{red,bold}Failed to stat path:{normal} %s\n", path);
        fossil_fstream_write(FOSSIL_STDERR, err_msg, fossil_io_cstring_length(err_msg), 1);
        fossil_io_cstring_free(err_msg);
        return errno;
    }

    cstring msg = fossil_io_cstring_format("{green,bold}Watching %s every %d seconds...{normal}\n", path, interval);
    fossil_fstream_write(FOSSIL_STDOUT, msg, fossil_io_cstring_length(msg), 1);
    fossil_io_cstring_free(msg);

    while (1) {
        sleep(interval);

        struct stat curr_stat;
        int rc = stat(path, &curr_stat);
        if (rc != 0) {
            if (errno == ENOENT && fossil_io_cstring_icontains(events, "delete")) {
                cstring del_msg = fossil_io_cstring_format("{red}File deleted:{normal} %s\n", path);
                fossil_fstream_write(FOSSIL_STDOUT, del_msg, fossil_io_cstring_length(del_msg), 1);
                fossil_io_cstring_free(del_msg);
            }
            continue;
        }

        // Check modification time
        if (curr_stat.st_mtime != prev_stat.st_mtime && fossil_io_cstring_icontains(events, "modify")) {
            cstring mod_msg = fossil_io_cstring_format("{yellow}File modified:{normal} %s\n", path);
            fossil_fstream_write(FOSSIL_STDOUT, mod_msg, fossil_io_cstring_length(mod_msg), 1);
            fossil_io_cstring_free(mod_msg);
        }

        // Check size change
        if (curr_stat.st_size != prev_stat.st_size && fossil_io_cstring_icontains(events, "modify")) {
            cstring size_msg = fossil_io_cstring_format("{cyan}File size changed:{normal} %s\n", path);
            fossil_fstream_write(FOSSIL_STDOUT, size_msg, fossil_io_cstring_length(size_msg), 1);
            fossil_io_cstring_free(size_msg);
        }

        prev_stat = curr_stat;
    }
#endif

    return 0;
}
