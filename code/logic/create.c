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

static int create_parent_dirs(const char *path)
{
    char dir_path[FOSSIL_FILESYS_MAX_PATH];
    int32_t rc = fossil_io_filesys_dirname(path, dir_path, sizeof(dir_path));
    if (rc < 0 || dir_path[0] == '\0')
        return 0; // root or error, nothing to do

    if (fossil_io_filesys_exists(dir_path) == 1)
        return 0; // already exists

    // Recursively create parent directories
    if (create_parent_dirs(dir_path) != 0)
        return 1;

    rc = fossil_io_filesys_dir_create(dir_path, false);
    if (rc < 0) {
        fossil_io_printf("{red}Error creating directory '%s': %s{normal}\n", dir_path, strerror(errno));
        return 1;
    }
    return 0;
}

int fossil_spino_create(const char *path, bool create_parents, const char *type)
{
    if (cunlikely(!path || !type))
    {
        fossil_io_printf("{red}Error: Path and type must be specified.{normal}\n");
        return 1;
    }

    if (create_parents)
    {
        if (cunlikely(create_parent_dirs(path) != 0))
            return 1;
    }

    int32_t exists = fossil_io_filesys_exists(path);
    if (exists == 1)
    {
        fossil_io_printf("{red}Error: '%s' already exists.{normal}\n", path);
        return 1;
    }
    else if (exists < 0)
    {
        fossil_io_printf("{red}Error: Unable to check existence of '%s'.{normal}\n", path);
        return 1;
    }

    if (strcmp(type, "file") == 0)
    {
        fossil_io_filesys_file_t file;
        int32_t rc = fossil_io_filesys_file_open(&file, path, "w");
        if (rc < 0)
        {
            fossil_io_printf("{red}Error creating file '%s': %s{normal}\n", path, strerror(errno));
            return 1;
        }
        fossil_io_filesys_file_close(&file);
        fossil_io_printf("{cyan}File '%s' created successfully.{normal}\n", path);
    }
    else if (strcmp(type, "dir") == 0)
    {
        int32_t rc = fossil_io_filesys_dir_create(path, false);
        if (rc < 0)
        {
            fossil_io_printf("{red}Error creating directory '%s': %s{normal}\n", path, strerror(errno));
            return 1;
        }
        fossil_io_printf("{blue}Directory '%s' created successfully.{normal}\n", path);
    }
    else
    {
        fossil_io_printf("{red}Error: Invalid type '%s'. Must be 'file' or 'dir'.{normal}\n", type);
        return 1;
    }

    return 0;
}
