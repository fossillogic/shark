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
#include <sys/stat.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

#include <dirent.h>

__attribute__((unused))
static void fossil_shark_watch_file(const char *path, const char *events, struct stat *prev_stat)
{
    struct stat curr_stat;
    int rc = stat(path, &curr_stat);
    if (rc != 0) {
        if (errno == ENOENT && fossil_io_cstring_icontains(events, "delete")) {
            cstring del_msg = fossil_io_cstring_format("{red}File deleted:{normal} %s\n", path);
            fossil_fstream_write(FOSSIL_STDOUT, del_msg, fossil_io_cstring_length(del_msg), 1);
            fossil_io_cstring_free(del_msg);
        }
        return;
    }

    // Check modification time
    if (curr_stat.st_mtime != prev_stat->st_mtime && fossil_io_cstring_icontains(events, "modify")) {
        cstring mod_msg = fossil_io_cstring_format("{yellow}File modified:{normal} %s\n", path);
        fossil_fstream_write(FOSSIL_STDOUT, mod_msg, fossil_io_cstring_length(mod_msg), 1);
        fossil_io_cstring_free(mod_msg);
    }

    // Check size change
    if (curr_stat.st_size != prev_stat->st_size && fossil_io_cstring_icontains(events, "modify")) {
        cstring size_msg = fossil_io_cstring_format("{cyan}File size changed:{normal} %s\n", path);
        fossil_fstream_write(FOSSIL_STDOUT, size_msg, fossil_io_cstring_length(size_msg), 1);
        fossil_io_cstring_free(size_msg);
    }

    *prev_stat = curr_stat;
}

#if !defined(_WIN32) && !defined(_WIN64)
__attribute__((unused))
static void fossil_shark_watch_dir(const char *dir_path, const char *events, int interval)
{
    DIR *dir = opendir(dir_path);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                fossil_shark_watch_dir(full_path, events, interval);
            } else {
                struct stat prev_stat = st;
                while (1) {
                    sleep(interval);
                    fossil_shark_watch_file(full_path, events, &prev_stat);
                }
            }
        }
    }
    closedir(dir);
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
        fossil_fstream_write(FOSSIL_STDERR, err_msg, fossil_io_cstring_length(err_msg), 1);
        fossil_io_cstring_free(err_msg);
        return GetLastError();
    }

    cstring msg = fossil_io_cstring_format("{green,bold}Watching %s every %d seconds...{normal}%s\n", path, interval, recursive ? " (recursive not implemented on Windows)" : "");
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
    struct stat st;
    if (stat(path, &st) != 0) {
        cstring err_msg = fossil_io_cstring_format("{red,bold}Failed to stat path:{normal} %s\n", path);
        fossil_fstream_write(FOSSIL_STDERR, err_msg, fossil_io_cstring_length(err_msg), 1);
        fossil_io_cstring_free(err_msg);
        return errno;
    }

    cstring msg = fossil_io_cstring_format("{green,bold}Watching %s every %d seconds...{normal}%s\n", path, interval, recursive ? " (recursive enabled)" : "");
    fossil_fstream_write(FOSSIL_STDOUT, msg, fossil_io_cstring_length(msg), 1);
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
