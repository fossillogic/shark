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
    int32_t rc = 0;

    // If src is not a directory, sync as a file
    if (fossil_io_dir_is_directory(src) != 1) {
        return sync_file(src, dest, update);
    }

    // Create destination directory if needed
    if (fossil_io_dir_exists(dest) != 1) {
        rc = fossil_io_dir_create(dest);
        if (rc != 0) return rc;
    }

    // Iterate source directory
    fossil_io_dir_iter_t it;
    rc = fossil_io_dir_iter_open(&it, src);
    if (rc != 0) { perror("Failed to open source dir"); return rc; }

    while (fossil_io_dir_iter_next(&it) == 1) {
        fossil_io_dir_entry_t *entry = &it.current;
        if (strcmp(entry->name, ".") == 0 || strcmp(entry->name, "..") == 0) continue;

        char dest_path[PATH_MAX_LEN];
        fossil_io_dir_join(dest, entry->name, dest_path, sizeof(dest_path));

        if (entry->type == 1) { // Directory
            if (recursive) {
                fossil_shark_sync(entry->path, dest_path, recursive, update, delete_flag);
            }
        } else if (entry->type == 0) { // File
            sync_file(entry->path, dest_path, update);
        }
        // Symlinks and other types can be handled here if needed
    }
    fossil_io_dir_iter_close(&it);

    // Delete extraneous files in dest
    if (delete_flag) {
        fossil_io_dir_iter_t dit;
        rc = fossil_io_dir_iter_open(&dit, dest);
        if (rc != 0) return rc;

        while (fossil_io_dir_iter_next(&dit) == 1) {
            fossil_io_dir_entry_t *dentry = &dit.current;
            if (strcmp(dentry->name, ".") == 0 || strcmp(dentry->name, "..") == 0) continue;

            char src_path[PATH_MAX_LEN];
            fossil_io_dir_join(src, dentry->name, src_path, sizeof(src_path));

            int exists = (dentry->type == 1)
                ? fossil_io_dir_exists(src_path)
                : fossil_io_dir_is_file(src_path);

            if (exists != 1) {
                if (dentry->type == 1) {
                    fossil_io_dir_remove_recursive(dentry->path);
                } else if (dentry->type == 0) {
                    fossil_io_file_delete(dentry->path);
                }
            }
        }
        fossil_io_dir_iter_close(&dit);
    }

    return 0;
}
