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

// Helper: regex match using fossil_io_regex functions
static bool regex_match(ccstring text, ccstring pattern) {
    if (!pattern) return true; // No pattern means match all
    
    fossil_io_regex_t *re = fossil_io_regex_compile(pattern, NULL, NULL);
    if (!re) return false;
    
    fossil_io_regex_match_t *match = NULL;
    int result = fossil_io_regex_match(re, text, &match);
    
    if (match) {
        fossil_io_regex_match_free(match);
    }
    fossil_io_regex_free(re);
    
    return result > 0;
}

// Helper: check if file extension matches filter
static bool extension_match(ccstring file_path, ccstring extensions) {
    if (!extensions) return true; // No filter means match all
    
    ccstring ext = fossil_io_path_get_extension(file_path);
    if (!ext) return false;
    
    // Parse comma-separated extensions
    char *ext_list = (char*)fossil_sys_memory_alloc(fossil_io_cstring_length(extensions) + 1);
    if (cunlikely(!ext_list)) return false;
    
    fossil_io_cstring_copy(ext_list, extensions);
    ccstring token = fossil_io_cstring_tokenize(ext_list, ",");
    bool found = false;
    
    while (token) {
        fossil_io_trim((char*)token);
        if (fossil_io_cstring_iequals(ext, token)) {
            found = true;
            break;
        }
        token = fossil_io_cstring_tokenize(NULL, ",");
    }
    
    fossil_sys_memory_free(ext_list);
    return found;
}

// Helper: search within file contents
static bool content_match(ccstring file_path, ccstring pattern, bool ignore_case, bool use_regex) {
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
        fossil_io_trim(line);
        if (use_regex ? regex_match(line, pattern) : str_match(line, pattern, ignore_case)) {
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
                            bool ignore_case, bool use_regex, ccstring glob_pattern,
                            ccstring extensions, ccstring size_range,
                            ccstring modified_range, ccstring hash_value,
                            bool follow_links, bool output_fson, int max_results,
                            bool use_parallel, int *results_count) {
    fossil_io_dir_iter_t it;
    int32_t rc = fossil_io_dir_iter_open(&it, path);
    if (rc != 0) {
        fossil_io_printf("{red}Error opening directory: %s{normal}\n", path);
        return rc;
    }

    while (fossil_io_dir_iter_next(&it) > 0) {
        if (max_results > 0 && *results_count >= max_results) break;
        
        fossil_io_dir_entry_t *entry = &it.current;
        // Skip . and ..
        if (fossil_io_cstring_equals(entry->name, ".") ||
            fossil_io_cstring_equals(entry->name, "..")) continue;

        if (entry->type == 1) { // Directory
            if (recursive && (!follow_links || !fossil_io_is_symlink(entry->path))) {
                search_recursive(entry->path, recursive, name_pattern, content_pattern,
                                ignore_case, use_regex, glob_pattern, extensions,
                                size_range, modified_range, hash_value, follow_links,
                                output_fson, max_results, use_parallel, results_count);
            }
        } else if (entry->type == 0) { // Regular file
            ccstring pattern = glob_pattern ? glob_pattern : name_pattern;
            bool name_matches = use_regex ? regex_match(entry->name, pattern) : 
                                           str_match(entry->name, pattern, ignore_case);
            
            if (name_matches &&
                extension_match(entry->path, extensions) &&
                content_match(entry->path, content_pattern, ignore_case, use_regex)) {
                if (output_fson) {
                    fossil_io_printf("object: { path: cstr: \"%s\", name: cstr: \"%s\" }\n", 
                                   entry->path, entry->name);
                } else {
                    fossil_io_printf("{cyan}Found:{normal} %s\n", entry->path);
                }
                (*results_count)++;
            }
        }
    }

    fossil_io_dir_iter_close(&it);
    return 0;
}

int fossil_shark_search(ccstring path, bool recursive,
                        ccstring name_pattern, ccstring content_pattern,
                        bool ignore_case, bool use_regex,
                        ccstring glob_pattern, ccstring extensions,
                        ccstring size_range, ccstring modified_range,
                        ccstring hash_value, bool follow_links,
                        bool output_fson, int max_results,
                        bool use_parallel) {
    if (!path) path = ".";
    int results_count = 0;
    return search_recursive(path, recursive, name_pattern, content_pattern, ignore_case,
                           use_regex, glob_pattern, extensions, size_range, modified_range,
                           hash_value, follow_links, output_fson, max_results, use_parallel,
                           &results_count);
}
