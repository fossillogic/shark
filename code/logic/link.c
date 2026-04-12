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
#include "fossil/code/link.h"

int fossil_spino_link(
    const char* source_file,
    const char* target_path,
    bool symbolic,
    bool hard,
    bool relative,
    bool overwrite
)
{
    if (!source_file || !target_path) return -1;

    // Check source exists
    if (!fossil_io_filesys_exists(source_file)) {
        fossil_io_fprintf(FOSSIL_IO_ERROR, "Source file does not exist: %s\n", source_file);
        return -2;
    }

    // If target exists
    if (fossil_io_filesys_exists(target_path)) {
        if (overwrite) {
            int rc = fossil_io_filesys_remove(target_path, false);
            if (rc != 0) return rc;
        } else {
            fossil_io_fprintf(FOSSIL_IO_ERROR, "Target already exists: %s\n", target_path);
            return -3;
        }
    }

    char final_source[FOSSIL_FILESYS_MAX_PATH];
    if (relative) {
        // Compute relative path
        char abs_source[FOSSIL_FILESYS_MAX_PATH] = {0};
        char abs_target[FOSSIL_FILESYS_MAX_PATH] = {0};
        fossil_io_filesys_abspath(source_file, abs_source, sizeof(abs_source));
        fossil_io_filesys_abspath(target_path, abs_target, sizeof(abs_target));

        // Simple relative path calculation (not fully robust, but works for same-dir)
        const char* src_ptr = abs_source;
        const char* tgt_ptr = abs_target;
        while (*src_ptr && *tgt_ptr && *src_ptr == *tgt_ptr) { src_ptr++; tgt_ptr++; }
        if (*tgt_ptr == '\0') strcpy(final_source, src_ptr);
        else strcpy(final_source, abs_source);
    } else {
        strncpy(final_source, source_file, sizeof(final_source));
    }

    int rc = -1;
    if (symbolic) {
        rc = fossil_io_filesys_link_create(final_source, target_path, true);
    } else if (hard) {
        rc = fossil_io_filesys_link_create(final_source, target_path, false);
    } else {
        fossil_io_fprintf(FOSSIL_IO_ERROR, "No link type specified (symbolic or hard)\n");
        return -4;
    }

    return rc;
}
