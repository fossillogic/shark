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

#define SWAP_PATH_MAX 4096

// Utility function to check if paths are on the same filesystem
static bool _is_same_filesystem(ccstring path1, ccstring path2) {
    struct stat stat1, stat2;
    if (stat(path1, &stat1) != 0 || stat(path2, &stat2) != 0) {
        return false;
    }
    return stat1.st_dev == stat2.st_dev;
}

// Utility function to display progress
static void _show_progress(ccstring message) {
    fossil_io_printf("[PROGRESS] %s\n", message);
    fossil_io_flush();
}

int fossil_shark_swap(ccstring path1, ccstring path2,
                      bool force, bool interactive, bool backup,
                      bool atomic, bool progress, bool dry_run,
                      ccstring temp_path, bool no_cross_device) {
    
    if (!path1 || !path2) {
        return 1;
    }
    
    // Check filesystem constraint
    if (no_cross_device && !_is_same_filesystem(path1, path2)) {
        fossil_io_printf("{red}Error: Paths are on different filesystems{reset}\n");
        return 1;
    }
    
    // Interactive confirmation
    if (interactive) {
        fossil_io_printf("Swap '%s' with '%s'? (y/n): ", path1, path2);
        int c = getchar();
        if (c != 'y' && c != 'Y') {
            if (!force) {
                return 1;
            }
        }
    }
    
    if (progress) {
        _show_progress("Starting swap operation");
    }
    
    // Create backups if requested
    if (backup) {
        if (fossil_io_file_backup(path1, ".bak") != 0 || 
            fossil_io_file_backup(path2, ".bak") != 0) {
            fossil_io_printf("{red}Error: Failed to create backups{reset}\n");
            if (!force) return 1;
        }
    }
    
    // Dry-run mode
    if (dry_run) {
        if (progress) {
            _show_progress("Dry-run: Would swap paths");
        }
        return 0;
    }
    
    // Use temporary path for atomic swap
    char temp[SWAP_PATH_MAX];
    if (atomic && temp_path) {
        snprintf(temp, sizeof(temp), "%s", temp_path);
    } else {
        fossil_io_dir_create_temp(temp, sizeof(temp));
    }
    
    // Perform swap: path1 -> temp, path2 -> path1, temp -> path2
    if (fossil_io_file_rename(path1, temp) != 0) {
        fossil_io_printf("{red}Error: Failed to move '%s' to temp{reset}\n", path1);
        return force ? 0 : 1;
    }
    
    if (fossil_io_file_rename(path2, path1) != 0) {
        fossil_io_file_rename(temp, path1);
        fossil_io_printf("{red}Error: Failed to move '%s' to '%s'{reset}\n", path2, path1);
        return force ? 0 : 1;
    }
    
    if (fossil_io_file_rename(temp, path2) != 0) {
        fossil_io_printf("{red}Error: Failed to move temp to '%s'{reset}\n", path2);
        return force ? 0 : 1;
    }
    
    if (progress) {
        _show_progress("Swap operation completed successfully");
    }
    
    return 0;
}
