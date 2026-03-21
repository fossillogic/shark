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

static int copy_file(ccstring src, ccstring dest, bool update, bool preserve,
                     bool checksum, bool dry_run)
{
    if (cunlikely(!cnotnull(src) || !cnotnull(dest)))
    {
        fossil_io_printf("{red}Error: Source and destination paths cannot be null{normal}\n");
        return 1;
    }

    fossil_io_filesys_obj_t src_obj, dest_obj;
    if (fossil_io_filesys_stat(src, &src_obj) < 0)
    {
        fossil_io_printf("{red}Error: Cannot stat source file '%s'{normal}\n", src);
        return 1;
    }

    if (dry_run)
    {
        fossil_io_printf("{cyan}[DRY RUN] Would copy file: %s -> %s{normal}\n", src, dest);
        return 0;
    }

    bool dest_exists = (fossil_io_filesys_stat(dest, &dest_obj) == 0);

    if (update && dest_exists)
    {
        char src_hash[128] = {0}, dest_hash[128] = {0};
        int src_hash_res = fossil_io_filesys_file_hash(src, (unsigned char*)src_hash, sizeof(src_hash), "xxhash64");
        int dest_hash_res = fossil_io_filesys_file_hash(dest, (unsigned char*)dest_hash, sizeof(dest_hash), "xxhash64");
        if (src_hash_res == 0 && dest_hash_res == 0 && memcmp(src_hash, dest_hash, sizeof(src_hash)) == 0)
        {
            fossil_io_printf("{cyan}Skipping '%s' - destination is up to date (hash match){normal}\n", src);
            return 0;
        }
    }

    fossil_io_filesys_file_t src_stream, dest_stream;

    if (fossil_io_filesys_file_open(&src_stream, src, "rb") < 0)
    {
        fossil_io_printf("{red}Error: Cannot open source file '%s'{normal}\n", src);
        return 1;
    }

    if (fossil_io_filesys_file_open(&dest_stream, dest, "wb") < 0)
    {
        fossil_io_printf("{red}Error: Cannot create destination file '%s'{normal}\n", dest);
        fossil_io_filesys_file_close(&src_stream);
        return 1;
    }

    fossil_io_printf("{cyan}Copying file: %s -> %s{normal}\n", src, dest);

    char buffer[8192];
    size_t n;
    while ((n = fossil_io_filesys_file_read(&src_stream, buffer, 1, sizeof(buffer))) > 0)
    {
        if (cunlikely(fossil_io_filesys_file_write(&dest_stream, buffer, 1, n) != n))
        {
            fossil_io_printf("{red}Error: Write failed for '%s'{normal}\n", dest);
            fossil_io_filesys_file_close(&src_stream);
            fossil_io_filesys_file_close(&dest_stream);
            return 1;
        }
    }

    fossil_io_filesys_file_close(&src_stream);
    fossil_io_filesys_file_close(&dest_stream);

    if (checksum)
    {
        char src_hash[128] = {0}, dest_hash[128] = {0};
        int src_hash_res = fossil_io_filesys_file_hash(src, (unsigned char*)src_hash, sizeof(src_hash), "xxhash64");
        int dest_hash_res = fossil_io_filesys_file_hash(dest, (unsigned char*)dest_hash, sizeof(dest_hash), "xxhash64");
        if (src_hash_res != 0 || dest_hash_res != 0 || memcmp(src_hash, dest_hash, sizeof(src_hash)) != 0)
        {
            fossil_io_printf("{red}Error: Checksum verification failed for '%s'{normal}\n", dest);
            return 1;
        }
        fossil_io_printf("{cyan}Checksum verified for '%s'{normal}\n", dest);
    }

    if (preserve)
    {
        // Only preserve permissions and timestamps if available
        if (fossil_io_filesys_stat(src, &src_obj) == 0)
        {
#ifndef _WIN32
            chmod(dest, src_obj.mode);
            struct utimbuf times = {src_obj.accessed_at, src_obj.modified_at};
            utime(dest, &times);
#endif
            fossil_io_printf("{cyan}Preserved permissions and timestamps for '%s'{normal}\n", dest);
        }
    }

    return 0;
}

static int copy_directory(ccstring src, ccstring dest,
                          bool recursive, bool update, bool preserve,
                          bool checksum, bool sparse, bool link, bool reflink,
                          bool progress, bool dry_run,
                          ccstring exclude_pattern, ccstring include_pattern)
{
    if (cunlikely(!cnotnull(src) || !cnotnull(dest)))
    {
        fossil_io_printf("{red}Error: Source and destination paths cannot be null{normal}\n");
        return 1;
    }

    fossil_io_filesys_obj_t src_obj;
    if (fossil_io_filesys_stat(src, &src_obj) < 0)
    {
        fossil_io_printf("{red}Error: Cannot stat source directory '%s'{normal}\n", src);
        return 1;
    }

    if (dry_run)
    {
        fossil_io_printf("{cyan}[DRY RUN] Would create directory: %s{normal}\n", dest);
        return 0;
    }

    fossil_io_printf("{cyan}Creating directory: %s{normal}\n", dest);
    int32_t dir_create_result = fossil_io_filesys_dir_create(dest, false);
    if (dir_create_result < 0)
    {
        fossil_io_printf("{red}Error: Cannot create directory '%s'{normal}\n", dest);
        return 1;
    }

    // List directory entries
    fossil_io_filesys_obj_t entries[256];
    size_t entry_count = 0;
    if (fossil_io_filesys_dir_list(src, entries, 256, &entry_count) < 0)
    {
        fossil_io_printf("{red}Error: Cannot list directory '%s'{normal}\n", src);
        return 1;
    }

    for (size_t i = 0; i < entry_count; ++i)
    {
        fossil_io_filesys_obj_t *entry = &entries[i];
        if (strcmp(entry->path, ".") == 0 || strcmp(entry->path, "..") == 0)
            continue;

        char dest_path[FOSSIL_FILESYS_MAX_PATH];
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, strrchr(entry->path, '/') ? strrchr(entry->path, '/') + 1 : entry->path);

        if (entry->type == FOSSIL_FILESYS_TYPE_DIR)
        {
            if (recursive)
            {
                if (copy_directory(entry->path, dest_path, recursive, update, preserve,
                                   checksum, sparse, link, reflink, progress, dry_run,
                                   exclude_pattern, include_pattern) != 0)
                {
                    return 1;
                }
            }
        }
        else if (entry->type == FOSSIL_FILESYS_TYPE_FILE)
        {
            if (copy_file(entry->path, dest_path, update, preserve,
                          checksum, dry_run) != 0)
            {
                return 1;
            }
        }
    }

    if (preserve)
    {
#ifndef _WIN32
        struct utimbuf times = {src_obj.accessed_at, src_obj.modified_at};
        utime(dest, &times);
#endif
    }

    return 0;
}

int fossil_shark_copy(ccstring src, ccstring dest,
                      bool recursive, bool update, bool preserve,
                      bool checksum, bool sparse, bool link, bool reflink,
                      bool progress, bool dry_run,
                      ccstring exclude_pattern, ccstring include_pattern)
{
    if (cunlikely(!cnotnull(src) || !cnotnull(dest)))
    {
        fossil_io_printf("{red}Error: Source and destination must be specified{normal}\n");
        return 1;
    }

    fossil_io_filesys_obj_t src_obj;
    if (fossil_io_filesys_stat(src, &src_obj) < 0)
    {
        fossil_io_printf("{red}Error: Cannot access source '%s'{normal}\n", src);
        return 1;
    }

    if (src_obj.type == FOSSIL_FILESYS_TYPE_DIR)
    {
        if (!recursive)
        {
            fossil_io_printf("{red}Error: Source is a directory. Use recursive flag to copy directories{normal}\n");
            return 1;
        }
        fossil_io_printf("{cyan}Starting recursive copy of directory: %s -> %s{normal}\n", src, dest);
        return copy_directory(src, dest, recursive, update, preserve,
                              checksum, sparse, link, reflink, progress, dry_run,
                              exclude_pattern, include_pattern);
    }
    else if (src_obj.type == FOSSIL_FILESYS_TYPE_FILE)
    {
        return copy_file(src, dest, update, preserve,
                         checksum, dry_run);
    }
    else
    {
        fossil_io_printf("{red}Error: Unsupported file type for '%s'{normal}\n", src);
        return 1;
    }
    return 0;
}
