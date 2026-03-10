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

    // Use temp path for atomic swap if provided
    cstring temp_file = temp_path ? (cstring)temp_path : 
                        fossil_io_cstring_format("%s.swap", norm_path1);
    
    if (!cnotnull(temp_file)) {
        fossil_io_printf("{red}Error: Failed to create temporary path.{normal}\n");
        fossil_io_cstring_free(norm_path1);
        fossil_io_cstring_free(norm_path2);
        return 1;
    }

    // Check cross-device constraint if enabled
    if (no_cross_device) {
        if (fossil_io_device_differs(norm_path1, norm_path2)) {
            fossil_io_printf("{red}Error: Paths are on different devices.{normal}\n");
            if (!temp_path) fossil_io_cstring_free(temp_file);
            fossil_io_cstring_free(norm_path1);
            fossil_io_cstring_free(norm_path2);
            return 1;
        }
    }

    // Show progress if enabled
    if (progress) {
        fossil_io_printf("{cyan}[PROGRESS] Starting swap operation...{normal}\n");
    }

    // Move path1 to temp
    if (fossil_io_file_move(norm_path1, temp_file) != 0) {
        fossil_io_printf("{red}Failed to move '%s' to temp: %s{normal}\n", norm_path1, strerror(errno));
        if (!temp_path) fossil_io_cstring_free(temp_file);
        fossil_io_cstring_free(norm_path1);
        fossil_io_cstring_free(norm_path2);
        return errno;
    }

    if (progress) fossil_io_printf("{cyan}[PROGRESS] Moved source to temp...{normal}\n");

    // Move path2 to path1
    if (fossil_io_file_move(norm_path2, norm_path1) != 0) {
        fossil_io_printf("{red}Failed to move '%s' to '%s': %s{normal}\n", norm_path2, norm_path1, strerror(errno));
        fossil_io_file_move(temp_file, norm_path1); // Rollback
        if (!temp_path) fossil_io_cstring_free(temp_file);
        fossil_io_cstring_free(norm_path1);
        fossil_io_cstring_free(norm_path2);
        return errno;
    }

    if (progress) fossil_io_printf("{cyan}[PROGRESS] Moved destination to source...{normal}\n");

    // Move temp to path2
    if (fossil_io_file_move(temp_file, norm_path2) != 0) {
        fossil_io_printf("{red}Failed to move temp to '%s': %s{normal}\n", norm_path2, strerror(errno));
        fossil_io_file_move(norm_path1, norm_path2); // Rollback
        fossil_io_file_move(temp_file, norm_path1); // Rollback
        if (!temp_path) fossil_io_cstring_free(temp_file);
        fossil_io_cstring_free(norm_path1);
        fossil_io_cstring_free(norm_path2);
        return errno;
    }

    if (atomic) {
        fossil_io_printf("{cyan}[ATOMIC] Swap completed atomically.{normal}\n");
    }

    fossil_io_printf("{cyan}Successfully swapped '%s' <-> '%s'{normal}\n", norm_path1, norm_path2);
    
    if (!temp_path) fossil_io_cstring_free(temp_file);
    fossil_io_cstring_free(norm_path1);
    fossil_io_cstring_free(norm_path2);
    return 0;
}
