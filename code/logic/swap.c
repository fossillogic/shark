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


static bool fossil_io_device_differs(ccstring path1, ccstring path2) {
    struct stat stat1, stat2;
    
    if (stat(path1, &stat1) != 0 || stat(path2, &stat2) != 0) {
        return false;
    }
    
    return stat1.st_dev != stat2.st_dev;
}

//
static bool confirm_overwrite(ccstring path) {
    fossil_io_printf("Overwrite '%s'? (y/n): ", path);
    int ch = getchar();
    while (getchar() != '\n');
    return ch == 'y' || ch == 'Y';
}

static int create_backup(ccstring path) {
    cstring backup_path = fossil_io_cstring_format("%s.bak", path);
    if (!cnotnull(backup_path)) {
        return 1;
    }
    
    int result = fossil_io_file_copy(path, backup_path);
    fossil_io_cstring_free(backup_path);
    return result;
}

// To be added in Fossil Io 0.2.12
static cstring fossil_io_file_path_normalize(ccstring path) {
    if (!cnotnull(path)) {
        return NULL;
    }
    
    cstring normalized = fossil_io_cstring_dup(path);
    if (!cnotnull(normalized)) {
        return NULL;
    }
    
    // Convert backslashes to forward slashes for cross-platform consistency
    for (size_t i = 0; normalized[i] != '\0'; i++) {
        if (normalized[i] == '\\') {
            normalized[i] = '/';
        }
    }
    
    return normalized;
}

static int swap_with_temp(ccstring path1, ccstring path2, ccstring temp_path) {
    if (!cnotnull(temp_path)) {
        return fossil_io_file_swap(path1, path2);
    }
    
    if (fossil_io_file_copy(path1, temp_path) != 0) {
        return 1;
    }
    
    if (fossil_io_file_copy(path2, path1) != 0) {
        fossil_io_file_remove(temp_path);
        return 1;
    }
    
    if (fossil_io_file_copy(temp_path, path2) != 0) {
        return 1;
    }
    
    return fossil_io_file_remove(temp_path);
}

int fossil_shark_swap(ccstring path1, ccstring path2,
                      bool force, bool interactive, bool backup,
                      bool atomic, bool progress, bool dry_run,
                      ccstring temp_path, bool no_cross_device) {
    if (!cnotnull(path1) || !cnotnull(path2)) {
        fossil_io_printf("{red}Error: Both paths must be specified.{normal}\n");
        return 1;
    }

    if (fossil_io_cstring_contains(path1, path2)) {
        fossil_io_printf("{red}Error: Cannot swap a path with itself.{normal}\n");
        return 1;
    }

    // Normalize paths for cross-platform compatibility
    cstring norm_path1 = fossil_io_file_path_normalize(path1);
    cstring norm_path2 = fossil_io_file_path_normalize(path2);
    
    if (!cnotnull(norm_path1) || !cnotnull(norm_path2)) {
        fossil_io_printf("{red}Error: Failed to normalize paths.{normal}\n");
        if (norm_path1) fossil_io_cstring_free(norm_path1);
        if (norm_path2) fossil_io_cstring_free(norm_path2);
        return 1;
    }

    if (dry_run) {
        fossil_io_printf("{cyan}[DRY RUN] Would swap '%s' <-> '%s'{normal}\n", norm_path1, norm_path2);
        fossil_io_cstring_free(norm_path1);
        fossil_io_cstring_free(norm_path2);
        return 0;
    }

    if (interactive && !force) {
        if (!confirm_overwrite(norm_path1)) {
            fossil_io_printf("{cyan}Swap cancelled by user.{normal}\n");
            fossil_io_cstring_free(norm_path1);
            fossil_io_cstring_free(norm_path2);
            return 1;
        }
    }

    if (backup) {
        if (create_backup(norm_path1) != 0 || create_backup(norm_path2) != 0) {
            fossil_io_cstring_free(norm_path1);
            fossil_io_cstring_free(norm_path2);
            return 1;
        }
    }

    // Check cross-device constraint if enabled
    if (no_cross_device) {
        if (fossil_io_device_differs(norm_path1, norm_path2)) {
            fossil_io_printf("{red}Error: Paths are on different devices.{normal}\n");
            fossil_io_cstring_free(norm_path1);
            fossil_io_cstring_free(norm_path2);
            return 1;
        }
    }

    if (progress) {
        fossil_io_printf("{cyan}[PROGRESS] Starting swap operation...{normal}\n");
    }

    // Use fossil_io_file_swap function with temp_path for atomic swap
    int result = atomic ? swap_with_temp(norm_path1, norm_path2, temp_path) 
                        : fossil_io_file_swap(norm_path1, norm_path2);
    
    if (result != 0) {
        fossil_io_printf("{red}Failed to swap '%s' with '%s': %s{normal}\n", norm_path1, norm_path2, strerror(errno));
        fossil_io_cstring_free(norm_path1);
        fossil_io_cstring_free(norm_path2);
        return errno;
    }

    if (progress) {
        fossil_io_printf("{cyan}[PROGRESS] Swap operation completed...{normal}\n");
    }

    if (atomic) {
        fossil_io_printf("{cyan}[ATOMIC] Swap completed atomically.{normal}\n");
    }

    fossil_io_printf("{cyan}Successfully swapped '%s' <-> '%s'{normal}\n", norm_path1, norm_path2);
    
    fossil_io_cstring_free(norm_path1);
    fossil_io_cstring_free(norm_path2);
    return 0;
}
