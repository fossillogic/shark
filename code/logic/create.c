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
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#ifdef _WIN32
#include <direct.h>
#endif

/**
 * Helper: create parent directories recursively
 */
static int create_parent_dirs(ccstring path) {
    cstring tmp = fossil_io_cstring_create_safe(path, 4096);
    if (!tmp) return 1;

    cstring dir_path = fossil_io_cstring_create_safe(dirname(tmp), 4096);
    fossil_io_cstring_free_safe(&tmp);
    
    if (!dir_path) return 1;

    struct stat st;
    if (stat(dir_path, &st) == 0) {
        fossil_io_cstring_free_safe(&dir_path);
        return 0; // already exists
    }

    // Recursively create parent directories
    if (create_parent_dirs(dir_path) != 0) {
        fossil_io_cstring_free_safe(&dir_path);
        return 1;
    }

#ifdef _WIN32
    if (_mkdir(dir_path) != 0) {
#else
    if (mkdir(dir_path, 0755) != 0) {
#endif
        if (errno != EEXIST) {
            fossil_io_printf("{red}Error creating directory '%s': %s{normal}\n", dir_path, strerror(errno));
            fossil_io_cstring_free_safe(&dir_path);
            return errno;
        }
    }
    
    fossil_io_cstring_free_safe(&dir_path);
    return 0;
}

/**
 * Create new directories or files with specified options
 */
int fossil_shark_create(ccstring path, bool create_parents,
                        ccstring type) {
    if (cunlikely(!path || !type)) {
        fossil_io_printf("{red}Error: Path and type must be specified.{normal}\n");
        return 1;
    }

    if (create_parents) {
        if (cunlikely(create_parent_dirs(path) != 0)) return 1;
    }

    if (fossil_fstream_file_exists(path)) {
        fossil_io_printf("{red}Error: '%s' already exists.{normal}\n", path);
        return 1;
    }

    if (fossil_io_cstring_equals(type, "file")) {
        fossil_io_file_t stream;
        if (cunlikely(fossil_fstream_open(&stream, path, "w") != 0)) {
            fossil_io_printf("{red}Error creating file '%s': %s{normal}\n", path, strerror(errno));
            return errno;
        }
        fossil_fstream_close(&stream);
        fossil_io_printf("{cyan}File '%s' created successfully.{normal}\n", path);
    } else if (fossil_io_cstring_equals(type, "dir")) {
#ifdef _WIN32
        if (cunlikely(_mkdir(path) != 0)) {
#else
        if (cunlikely(mkdir(path, 0755) != 0)) {
#endif
            fossil_io_printf("{red}Error creating directory '%s': %s{normal}\n", path, strerror(errno));
            return errno;
        }
        fossil_io_printf("{blue}Directory '%s' created successfully.{normal}\n", path);
    } else {
        fossil_io_printf("{red}Error: Invalid type '%s'. Must be 'file' or 'dir'.{normal}\n", type);
        return 1;
    }

    return 0;
}
