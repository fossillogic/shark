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
#include "fossil/code/sync.h"

#define PATH_MAX_LEN 1024

static int compute_file_hash(ccstring path, char *hash_out, size_t hash_out_len)
{
    // Use SHA256 for demonstration; adjust as needed
    unsigned char hash_bin[32];
    int rc = fossil_io_filesys_file_hash(path, hash_bin, sizeof(hash_bin), "sha256");
    if (rc != 0)
        return rc;

    // Convert binary hash to hex string
    for (size_t i = 0; i < sizeof(hash_bin) && (i * 2 + 1) < hash_out_len; ++i)
        sprintf(hash_out + i * 2, "%02x", hash_bin[i]);
    hash_out[sizeof(hash_bin) * 2] = '\0';
    return 0;
}

static int sync_file(ccstring src, ccstring dest, bool update)
{
    fossil_io_filesys_obj_t src_obj, dest_obj;
    int rc = fossil_io_filesys_stat(src, &src_obj);
    if (rc != 0)
    {
        perror("stat src");
        return rc;
    }

    rc = fossil_io_filesys_stat(dest, &dest_obj);
    bool dest_exists = (rc == 0);

    if (dest_exists && update)
    {
        if (dest_obj.modified_at >= src_obj.modified_at)
        {
            // Destination is newer, skip
            return 0;
        }
    }

    // Compare hashes, skip copy if identical
    char src_hash[65], dest_hash[65];
    if (compute_file_hash(src, src_hash, sizeof(src_hash)) == 0 &&
        dest_exists &&
        compute_file_hash(dest, dest_hash, sizeof(dest_hash)) == 0)
    {
        if (strcmp(src_hash, dest_hash) == 0)
        {
            // Files are identical, skip copy
            return 0;
        }
    }

    return fossil_io_filesys_copy(src, dest, true);
}

// Main sync function
int fossil_spino_sync(ccstring src, ccstring dest,
                      bool recursive, bool update, bool delete_flag)
{
    int32_t rc = 0;
    fossil_io_filesys_obj_t src_obj;
    rc = fossil_io_filesys_stat(src, &src_obj);
    if (rc != 0)
        return rc;

    if (src_obj.type != FOSSIL_FILESYS_TYPE_DIR)
    {
        return sync_file(src, dest, update);
    }

    // Create destination directory if needed
    if (fossil_io_filesys_exists(dest) != 1)
    {
        rc = fossil_io_filesys_dir_create(dest, false);
        if (rc != 0)
            return rc;
    }

    // List source directory entries
    fossil_io_filesys_obj_t entries[256];
    size_t entry_count = 0;
    rc = fossil_io_filesys_dir_list(src, entries, 256, &entry_count);
    if (rc != 0)
        return rc;

    for (size_t i = 0; i < entry_count; ++i)
    {
        fossil_io_filesys_obj_t *entry = &entries[i];
        if (strcmp(entry->path, ".") == 0 || strcmp(entry->path, "..") == 0)
            continue;

        char dest_path[FOSSIL_FILESYS_MAX_PATH];
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->path + strlen(src) + 1);

        if (entry->type == FOSSIL_FILESYS_TYPE_DIR)
        {
            if (recursive)
            {
                fossil_spino_sync(entry->path, dest_path, recursive, update, delete_flag);
            }
        }
        else if (entry->type == FOSSIL_FILESYS_TYPE_FILE)
        {
            sync_file(entry->path, dest_path, update);
        }
        // Symlinks and other types can be handled here if needed
    }

    // Delete extraneous files in dest
    if (delete_flag)
    {
        fossil_io_filesys_obj_t dest_entries[256];
        size_t dest_entry_count = 0;
        rc = fossil_io_filesys_dir_list(dest, dest_entries, 256, &dest_entry_count);
        if (rc != 0)
            return rc;

        for (size_t i = 0; i < dest_entry_count; ++i)
        {
            fossil_io_filesys_obj_t *dentry = &dest_entries[i];
            if (strcmp(dentry->path, ".") == 0 || strcmp(dentry->path, "..") == 0)
                continue;

            char src_path[FOSSIL_FILESYS_MAX_PATH];
            snprintf(src_path, sizeof(src_path), "%s/%s", src, dentry->path + strlen(dest) + 1);

            int exists = fossil_io_filesys_exists(src_path);

            if (exists != 1)
            {
                fossil_io_filesys_remove(dentry->path, dentry->type == FOSSIL_FILESYS_TYPE_DIR);
            }
        }
    }

    return 0;
}
