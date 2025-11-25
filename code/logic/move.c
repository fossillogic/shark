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
#include <errno.h>

// Normalize file paths for cross-platform compatibility
cstring fossil_io_file_path_normalize(ccstring path) {
    if (!cnotnull(path)) return cnull;

    cstring norm_path = (cstring)fossil_sys_memory_alloc(strlen(path) + 1);
    if (cunlikely(!cnotnull(norm_path))) {
        return cnull;
    }

    size_t len = strlen(path);
    for (size_t i = 0; i < len; i++) {
#ifdef _WIN32
        norm_path[i] = (path[i] == '/') ? '\\' : path[i];
#else
        norm_path[i] = (path[i] == '\\') ? '/' : path[i];
#endif
    }
    norm_path[len] = cterm;
    return norm_path;
}

static int create_backup(ccstring dest) {
    cstring backup_path = fossil_io_cstring_format("%s.bak", dest);
    if (!cnotnull(backup_path)) {
        fossil_io_printf("{red}Failed to create backup path{normal}\n");
        return -1;
    }
    
    if (fossil_io_file_backup(dest, ".bak") != 0) {
        fossil_io_printf("{red}Failed to create backup: %s{normal}\n", strerror(errno));
        fossil_io_cstring_free(backup_path);
        return errno;
    }
    
    fossil_io_printf("{cyan}Backup created: %s{normal}\n", backup_path);
    fossil_io_cstring_free(backup_path);
    return 0;
}

static bool confirm_overwrite(ccstring dest) {
    char *answer = (char *)fossil_sys_memory_alloc(8);
    if (!cnotnull(answer)) {
        return false;
    }
    
    fossil_io_printf("{blue}Overwrite '%s'? [y/N]: {normal}", dest);
    
    if (fossil_io_gets(answer, 8) != 0) {
        fossil_sys_memory_free(answer);
        return false;
    }
    
    fossil_io_trim(answer);
    bool result = (answer[0] == 'y' || answer[0] == 'Y');
    fossil_sys_memory_free(answer);
    cdrop(answer);
    return result;
}

/**
 * Move or rename files and directories
 */
int fossil_shark_move(ccstring src, ccstring dest,
                      bool force, bool interactive, bool backup) {
    if (!cnotnull(src) || !cnotnull(dest)) {
        fossil_io_printf("{red}Error: Source and destination paths must be specified.{normal}\n");
        return 1;
    }

    // Normalize paths for cross-platform compatibility
    cstring norm_src = fossil_io_file_path_normalize(src);
    cstring norm_dest = fossil_io_file_path_normalize(dest);
    
    if (!cnotnull(norm_src) || !cnotnull(norm_dest)) {
        fossil_io_printf("{red}Error: Failed to normalize paths.{normal}\n");
        if (norm_src) fossil_io_cstring_free(norm_src);
        if (norm_dest) fossil_io_cstring_free(norm_dest);
        return 1;
    }

    bool dest_exists = fossil_io_file_file_exists(norm_dest);

    if (clikely(dest_exists)) {
        if (backup) {
            if (create_backup(norm_dest) != 0) {
                fossil_io_cstring_free(norm_src);
                fossil_io_cstring_free(norm_dest);
                return 1;
            }
        }

        if (interactive && !force) {
            if (!confirm_overwrite(norm_dest)) {
                fossil_io_printf("{cyan}Move cancelled by user.{normal}\n");
                fossil_io_cstring_free(norm_src);
                fossil_io_cstring_free(norm_dest);
                return 1;
            }
        }

        if (cunlikely(!force && !interactive && !backup)) {
            fossil_io_printf("{red}Error: Destination exists. Use --force, --interactive, or --backup.{normal}\n");
            fossil_io_cstring_free(norm_src);
            fossil_io_cstring_free(norm_dest);
            return 1;
        }
    }

    if (fossil_io_file_rename(norm_src, norm_dest) != 0) {
        fossil_io_printf("{red}Failed to move/rename: %s{normal}\n", strerror(errno));
        fossil_io_cstring_free(norm_src);
        fossil_io_cstring_free(norm_dest);
        return errno;
    }

    fossil_io_printf("{cyan}Successfully moved '%s' to '%s'{normal}\n", norm_src, norm_dest);
    fossil_io_cstring_free(norm_src);
    fossil_io_cstring_free(norm_dest);
    return 0;
}
