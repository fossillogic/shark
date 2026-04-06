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

#define MAX_PATH_LEN FOSSIL_FILESYS_MAX_PATH
#define HASH_STR_LEN 65  // e.g., SHA256 in hex + null terminator

typedef struct duplicate_node {
    char path[MAX_PATH_LEN];
    struct duplicate_node *next;
} duplicate_node_t;

/**
 * Internal helper: compute hash of a file
 */
static int compute_file_hash(const char *path, char *hash_out, size_t hash_len) {
    fossil_io_filesys_file_t file;
    if (fossil_io_filesys_file_open(&file, path, "rb") < 0) return -1;

    size_t buffer_size = file.base.size;
    if (buffer_size == 0) buffer_size = 8192;

    void *buffer = malloc(buffer_size);
    if (!buffer) {
        fossil_io_filesys_file_close(&file);
        return -1;
    }

    size_t read_bytes = fossil_io_filesys_file_read(&file, buffer, 1, buffer_size);
    int res = fossil_cryptic_hash_compute(
        "crc32", "u32", "hex", hash_out, hash_len, buffer, read_bytes
    );

    free(buffer);
    fossil_io_filesys_file_close(&file);
    return res;
}

/**
 * Internal helper: recursively walk directory and apply callback
 */
static int walk_dir_recursive(const char *path, int (*callback)(const fossil_io_filesys_obj_t *, void *), void *user_data) {
    return fossil_io_filesys_dir_walk(path, callback, user_data);
}

/**
 * Deduplicate files in a directory (optionally recursive)
 */
int32_t fossil_io_filesys_deduplicate(const char *path, bool recursive) {
    typedef struct hash_entry {
        char hash[HASH_STR_LEN];
        duplicate_node_t *files;
        struct hash_entry *next;
    } hash_entry_t;

    hash_entry_t *hash_table = NULL;
    int duplicates_removed = 0;

    // Callback for dir_walk
    int dedupe_callback(const fossil_io_filesys_obj_t *obj, void *user_data) {
        hash_entry_t **table = (hash_entry_t **)user_data;
        if (obj->type != FOSSIL_FILESYS_TYPE_FILE) return 0;

        char hash[HASH_STR_LEN] = {0};
        if (compute_file_hash(obj->path, hash, sizeof(hash)) != 0) return 0;

        // search hash table
        hash_entry_t *entry = *table;
        while (entry) {
            if (strcmp(entry->hash, hash) == 0) break;
            entry = entry->next;
        }

        if (entry) {
            // duplicate found -> remove
            if (fossil_io_filesys_remove(obj->path, false) == 0) {
                duplicates_removed++;
            }
            return 0;
        }

        // new hash -> add to table
        hash_entry_t *new_entry = malloc(sizeof(hash_entry_t));
        if (!new_entry) return 0;
        strncpy(new_entry->hash, hash, HASH_STR_LEN);
        new_entry->files = NULL;
        new_entry->next = *table;
        *table = new_entry;

        return 0;
    }

    // Walk the directory tree
    if (recursive) {
        walk_dir_recursive(path, dedupe_callback, &hash_table);
    } else {
        fossil_io_filesys_obj_t entries[1024];
        size_t count = 0;
        if (fossil_io_filesys_dir_list(path, entries, 1024, &count) == 0) {
            for (size_t i = 0; i < count; ++i) dedupe_callback(&entries[i], &hash_table);
        }
    }

    // Free hash table
    hash_entry_t *curr = hash_table;
    while (curr) {
        hash_entry_t *tmp = curr;
        curr = curr->next;
        free(tmp);
    }

    return duplicates_removed;
}
