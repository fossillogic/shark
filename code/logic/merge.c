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


typedef int (*pattern_matcher)(const char *filename, const char *pattern);

static int matches_pattern(const char *filename, const char *pattern) {
    if (!pattern) return 1;
    return fossil_io_cstring_icontains(filename, pattern);
}

static int should_process_file(const char *filename, const char *exclude, const char *include) {
    if (include && !matches_pattern(filename, include)) return 0;
    if (exclude && matches_pattern(filename, exclude)) return 0;
    return 1;
}

static int copy_file(const char *src, const char *dest, bool force) {
    fossil_io_file_t src_stream = {0};
    fossil_io_file_t dest_stream = {0};
    
    if (fossil_io_file_open(&src_stream, src, "rb") != 0) {
        return 1;
    }
    
    if (!force && fossil_io_file_file_exists(dest)) {
        fossil_io_file_close(&src_stream);
        return 2;
    }
    
    if (fossil_io_file_open(&dest_stream, dest, "wb") != 0) {
        fossil_io_file_close(&src_stream);
        return 1;
    }
    
    char buffer[8192];
    size_t bytes;
    while ((bytes = fossil_io_file_read(&src_stream, buffer, 1, sizeof(buffer))) > 0) {
        fossil_io_file_write(&dest_stream, buffer, 1, bytes);
    }
    
    fossil_io_file_close(&src_stream);
    fossil_io_file_close(&dest_stream);
    return 0;
}

static int merge_file(const char *src, const char *dest, bool force) {
    fossil_io_file_t src_stream = {0};
    fossil_io_file_t dest_stream = {0};

    if (fossil_io_file_open(&src_stream, src, "rb") != 0) {
        return 1;
    }

    if (!force && fossil_io_file_file_exists(dest)) {
        fossil_io_file_close(&src_stream);
        return 2;
    }

    if (fossil_io_file_open(&dest_stream, dest, "ab") != 0) {
        fossil_io_file_close(&src_stream);
        return 1;
    }

    char buffer[8192];
    size_t bytes;

    while ((bytes = fossil_io_file_read(&src_stream, buffer, 1, sizeof(buffer))) > 0) {
        fossil_io_file_write(&dest_stream, buffer, 1, bytes);
    }

    fossil_io_file_close(&src_stream);
    fossil_io_file_close(&dest_stream);

    return 0;
}

int fossil_shark_merge(const char **paths, int num_paths, ccstring dest,
                       bool force, bool interactive, bool backup,
                       ccstring strategy, bool progress, bool dry_run,
                       ccstring exclude_pattern, ccstring include_pattern) {
    if (!paths || !dest || num_paths <= 0) return 1;
    
    fossil_io_dir_create(dest);
    
    for (int i = 0; i < num_paths; i++) {
        if (!should_process_file(paths[i], exclude_pattern, include_pattern)) {
            continue;
        }
        
        if (interactive) {
            fossil_io_printf("{cyan}Merge %s? (y/n): {reset}", paths[i]);
            fossil_io_flush();
            int c = getchar();
            if (c != 'y' && c != 'Y') continue;
        }
        
        if (progress) fossil_io_printf("{green}✓ Processing:{reset} %s\n", paths[i]);
        
        if (dry_run) fossil_io_printf("{yellow}⚠ Dry run:{reset} would merge '%s' into '%s'\n", paths[i], dest);
        
        char dest_path[1024];
        fossil_io_dir_join(dest, paths[i], dest_path, sizeof(dest_path));
        
        if (backup && fossil_io_file_file_exists(dest_path)) {
            char backup_path[1024];

            size_t len = strlen(dest_path);
            if (len + 4 >= sizeof(backup_path)) {
                fossil_io_printf("{red}Path too long for backup:{reset} %s\n", dest_path);
                continue;
            }
            
            memcpy(backup_path, dest_path, len);
            memcpy(backup_path + len, ".bak", 5); // includes null terminator
            
            copy_file(dest_path, backup_path, true);
        }
        
        int result = merge_file(paths[i], dest_path, force);
        
        if (result != 0) {
            if (fossil_io_cstring_iequals(strategy, "skip")) {
                fossil_io_printf("{yellow}⊘ Skipped:{reset} %s\n", paths[i]);
                continue;
            } else if (fossil_io_cstring_iequals(strategy, "overwrite")) {
                fossil_io_printf("{bold}↻ Overwriting:{reset} %s\n", paths[i]);
                copy_file(paths[i], dest_path, true);
            } else if (fossil_io_cstring_iequals(strategy, "merge")) {
                fossil_io_printf("{blue}⇄ Merging:{reset} %s\n", paths[i]);
                merge_file(paths[i], dest_path, true);
            } else if (fossil_io_cstring_iequals(strategy, "abort")) {
                fossil_io_printf("{red,bold}✗ Error:{reset} Merge aborted at %s\n", paths[i]);
                return 1;
            }
        }
    }
    
    return 0;
}
