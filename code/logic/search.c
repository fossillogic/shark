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


// Helper: case-insensitive string match using fossil_io_cstring functions
static bool str_match(ccstring str, ccstring pattern, bool ignore_case) {
    if (!pattern) return true; // No pattern means match all
    
    if (!ignore_case) {
        return fossil_io_cstring_contains(str, pattern);
    } else {
        return fossil_io_cstring_icontains(str, pattern);
    }
}

// Helper: search within file contents
static bool content_match(ccstring file_path, ccstring pattern, bool ignore_case) {
    if (!pattern) return true; // No pattern means match all

    fossil_io_file_t stream;
    if (fossil_io_file_open(&stream, file_path, "r") != 0) {
        return false;
    }

    char *line = (char*)fossil_sys_memory_alloc(4096);
    if (cunlikely(!line)) {
        fossil_io_file_close(&stream);
        return false;
    }
    
    bool found = false;
    while (fossil_io_gets_from_stream(line, 4096, &stream)) {
        fossil_io_trim(line); // Trim whitespace from line
        if (str_match(line, pattern, ignore_case)) {
            found = true;
            break;
        }
    }

    fossil_sys_memory_free(line);
    fossil_io_file_close(&stream);
    return found;
}

static int search_recursive(ccstring path, bool recursive,
                            ccstring name_pattern, ccstring content_pattern,
                            bool ignore_case) {
    fossil_io_dir_iter_t it;
    int32_t rc = fossil_io_dir_iter_open(&it, path);
    if (rc != 0) {
        fossil_io_printf("{red}Error opening directory: %s{normal}\n", path);
        return rc;
    }

    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *entry = &it.current;
        // Skip . and ..
        if (fossil_io_cstring_equals(entry->name, ".") ||
            fossil_io_cstring_equals(entry->name, "..")) continue;

        if (entry->type == 1) { // Directory
            if (recursive) search_recursive(entry->path, recursive, name_pattern, content_pattern, ignore_case);
        } else if (entry->type == 0) { // Regular file
            if (str_match(entry->name, name_pattern, ignore_case) &&
                content_match(entry->path, content_pattern, ignore_case)) {
                fossil_io_printf("{cyan}%s{normal}\n", entry->path);
            }
        }
        // Ignore symlinks and other types for search
    }

    fossil_io_dir_iter_close(&it);
    return 0;
}

/**
 * Search for files by name patterns or content matching
 */
int fossil_shark_search(ccstring path, bool recursive,
                        ccstring name_pattern, ccstring content_pattern,
                        bool ignore_case) {
    if (!path) path = ".";
    return search_recursive(path, recursive, name_pattern, content_pattern, ignore_case);
}
