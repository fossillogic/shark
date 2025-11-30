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
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <utime.h>
#if defined(_WIN32)
#include <windows.h>
#include <io.h>
#include <direct.h>
#endif

#define PATH_MAX_LEN 1024


static int copy_file(ccstring src_path, ccstring dest_path) {
    int32_t rc = fossil_io_file_copy(src_path, dest_path);
    if (rc != 0) {
        perror("Failed to copy file");
        return rc;
    }
    return 0;
}

static int get_mod_time(ccstring path, time_t *mod_time) {
#if defined(_WIN32)
    struct _stat st;
    if (_stat(path, &st) != 0) return errno;
    *mod_time = st.st_mtime;
#else
    struct stat st;
    if (stat(path, &st) != 0) return errno;
    *mod_time = st.st_mtime;
#endif
    return 0;
}

static int is_directory(ccstring path) {
#if defined(_WIN32)
    struct _stat st;
    if (_stat(path, &st) != 0) return 0;
    return (st.st_mode & _S_IFDIR) != 0;
#else
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return S_ISDIR(st.st_mode);
#endif
}

static int make_dir(ccstring path) {
#if defined(_WIN32)
    if (_mkdir(path) != 0 && errno != EEXIST) {
        perror("Failed to create directory");
        return errno;
    }
#else
    if (mkdir(path, 0755) != 0 && errno != EEXIST) {
        perror("Failed to create directory");
        return errno;
    }
#endif
    return 0;
}

static int sync_file(ccstring src, ccstring dest, bool update) {
    time_t src_mtime, dest_mtime;
    if (get_mod_time(src, &src_mtime) != 0) { perror("stat src"); return errno; }

    if (get_mod_time(dest, &dest_mtime) == 0 && update) {
        if (dest_mtime >= src_mtime) {
            // Destination is newer, skip
            return 0;
        }
    }
    
    return copy_file(src, dest);
}

// Main sync function
int fossil_shark_sync(ccstring src, ccstring dest,
                      bool recursive, bool update, bool delete_flag) 
{
    if (!is_directory(src)) {
        // Sync a single file
        return sync_file(src, dest, update);
    }

    // Create destination directory if needed
    if (!is_directory(dest)) {
        if (make_dir(dest) != 0) return errno;
    }

    // Sync directory
#if defined(_WIN32)
    WIN32_FIND_DATAA find_data;
    char search_path[PATH_MAX_LEN];
    snprintf(search_path, sizeof(search_path), "%s\\*", src);
    HANDLE hFind = FindFirstFileA(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) { perror("Failed to open source dir"); return errno; }
    do {
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) continue;

        char src_path[PATH_MAX_LEN], dest_path[PATH_MAX_LEN];
        snprintf(src_path, sizeof(src_path), "%s\\%s", src, find_data.cFileName);
        snprintf(dest_path, sizeof(dest_path), "%s\\%s", dest, find_data.cFileName);

        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (recursive) {
                fossil_shark_sync(src_path, dest_path, recursive, update, delete_flag);
            }
        } else {
            sync_file(src_path, dest_path, update);
        }
    } while (FindNextFileA(hFind, &find_data));
    FindClose(hFind);
#else
    DIR *dir = opendir(src);
    if (!dir) { perror("Failed to open source dir"); return errno; }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char src_path[PATH_MAX_LEN], dest_path[PATH_MAX_LEN];
        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        struct stat st_entry;
        if (stat(src_path, &st_entry) != 0) { perror("stat entry"); continue; }

        if (S_ISDIR(st_entry.st_mode)) {
            if (recursive) {
                fossil_shark_sync(src_path, dest_path, recursive, update, delete_flag);
            }
        } else {
            sync_file(src_path, dest_path, update);
        }
    }
    closedir(dir);
#endif

    if (delete_flag) {
#if defined(_WIN32)
        snprintf(search_path, sizeof(search_path), "%s\\*", dest);
        hFind = FindFirstFileA(search_path, &find_data);
        if (hFind == INVALID_HANDLE_VALUE) return errno;
        do {
            if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) continue;

            char dest_path[PATH_MAX_LEN], src_path[PATH_MAX_LEN];
            snprintf(dest_path, sizeof(dest_path), "%s\\%s", dest, find_data.cFileName);
            snprintf(src_path, sizeof(src_path), "%s\\%s", src, find_data.cFileName);

            if (GetFileAttributesA(src_path) == INVALID_FILE_ATTRIBUTES) {
                // File does not exist in source, delete
                if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    // Remove directory
                    _rmdir(dest_path);
                } else {
                    if (fossil_io_file_delete(dest_path) != 0) perror("Failed to delete extraneous file");
                }
            }
        } while (FindNextFileA(hFind, &find_data));
        FindClose(hFind);
#else
        DIR *dest_dir = opendir(dest);
        if (!dest_dir) return errno;

        while ((entry = readdir(dest_dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            char dest_path[PATH_MAX_LEN], src_path[PATH_MAX_LEN];
            snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);
            snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);

            if (access(src_path, F_OK) != 0) {
                // File does not exist in source, delete
                struct stat st_entry;
                if (stat(dest_path, &st_entry) == 0 && S_ISDIR(st_entry.st_mode)) {
                    rmdir(dest_path);
                } else {
                    if (fossil_io_file_delete(dest_path) != 0) perror("Failed to delete extraneous file");
                }
            }
        }
        closedir(dest_dir);
#endif
    }

    return 0;
}
