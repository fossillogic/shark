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

static bool confirm_swap(ccstring path1, ccstring path2) {
    char *answer = (char *)fossil_sys_memory_alloc(8);
    if (!cnotnull(answer)) {
        return false;
    }
    
    fossil_io_printf("{blue}Swap '%s' and '%s'? [y/N]: {normal}", path1, path2);
    
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

static bool check_same_device(ccstring path1, ccstring path2) {
    struct stat stat1, stat2;
    if (stat(path1, &stat1) != 0 || stat(path2, &stat2) != 0) {
        return false;
    }
    return stat1.st_dev == stat2.st_dev;
}

int fossil_shark_swap(ccstring path1, ccstring path2,
                      bool force, bool interactive, bool backup,
                      bool atomic, bool progress, bool dry_run,
                      ccstring temp_path, bool no_cross_device) {
    if (!cnotnull(path1) || !cnotnull(path2)) {
        fossil_io_printf("{red}Error: Both paths must be specified.{normal}\n");
        return 1;
    }

    // Normalize paths
    cstring norm_path1 = fossil_io_file_path_normalize(path1);
    cstring norm_path2 = fossil_io_file_path_normalize(path2);
    
    if (!cnotnull(norm_path1) || !cnotnull(norm_path2)) {
        fossil_io_printf("{red}Error: Failed to normalize paths.{normal}\n");
        if (norm_path1) fossil_io_cstring_free(norm_path1);
        if (norm_path2) fossil_io_cstring_free(norm_path2);
        return 1;
    }

    // Check cross-device constraint
    if (no_cross_device && !check_same_device(norm_path1, norm_path2)) {
        fossil_io_printf("{red}Error: Paths are on different filesystems.{normal}\n");
        fossil_io_cstring_free(norm_path1);
        fossil_io_cstring_free(norm_path2);
        return 1;
    }

    if (dry_run) {
        fossil_io_printf("{cyan}[DRY RUN] Would swap '%s' and '%s'{normal}\n", norm_path1, norm_path2);
        fossil_io_cstring_free(norm_path1);
        fossil_io_cstring_free(norm_path2);
        return 0;
    }

    // Interactive confirmation
    if (interactive && !force) {
        if (!confirm_swap(norm_path1, norm_path2)) {
            fossil_io_printf("{cyan}Swap cancelled by user.{normal}\n");
            fossil_io_cstring_free(norm_path1);
            fossil_io_cstring_free(norm_path2);
            return 1;
        }
    }

    // Create backups if requested
    if (backup) {
        if (create_backup(norm_path1) != 0 || create_backup(norm_path2) != 0) {
            fossil_io_cstring_free(norm_path1);
            fossil_io_cstring_free(norm_path2);
            return 1;
        }
    }

    // Determine temp directory
    cstring swap_temp = cnotnull(temp_path) ? (cstring)temp_path : (cstring)".";
    cstring temp_file = fossil_io_cstring_format("%s/.fossil_swap_temp_XXXXXX", swap_temp);
    
    if (!cnotnull(temp_file)) {
        fossil_io_printf("{red}Error: Failed to allocate temp path.{normal}\n");
        fossil_io_cstring_free(norm_path1);
        fossil_io_cstring_free(norm_path2);
        return 1;
    }

    // Perform swap using temporary file
    if (progress) {
        fossil_io_printf("{cyan}Swapping files: 33%%{normal}\n");
    }

    if (fossil_io_file_rename(norm_path1, temp_file) != 0) {
        fossil_io_printf("{red}Failed to move '%s' to temp: %s{normal}\n", norm_path1, strerror(errno));
        fossil_io_cstring_free(norm_path1);
        fossil_io_cstring_free(norm_path2);
        fossil_io_cstring_free(temp_file);
        return errno;
    }

    if (progress) {
        fossil_io_printf("{cyan}Swapping files: 66%%{normal}\n");
    }

    if (fossil_io_file_rename(norm_path2, norm_path1) != 0) {
        fossil_io_printf("{red}Failed to move '%s' to '%s': %s{normal}\n", norm_path2, norm_path1, strerror(errno));
        fossil_io_file_rename(temp_file, norm_path1);
        fossil_io_cstring_free(norm_path1);
        fossil_io_cstring_free(norm_path2);
        fossil_io_cstring_free(temp_file);
        return errno;
    }

    if (fossil_io_file_rename(temp_file, norm_path2) != 0) {
        fossil_io_printf("{red}Failed to move temp to '%s': %s{normal}\n", norm_path2, strerror(errno));
        fossil_io_file_rename(norm_path1, norm_path2);
        fossil_io_file_rename(temp_file, norm_path1);
        fossil_io_cstring_free(norm_path1);
        fossil_io_cstring_free(norm_path2);
        fossil_io_cstring_free(temp_file);
        return errno;
    }

    if (progress) {
        fossil_io_printf("{cyan}Swapping files: 100%%{normal}\n");
    }

    fossil_io_printf("{cyan}Successfully swapped '%s' and '%s'{normal}\n", norm_path1, norm_path2);
    fossil_io_cstring_free(norm_path1);
    fossil_io_cstring_free(norm_path2);
    fossil_io_cstring_free(temp_file);
    return 0;
}
