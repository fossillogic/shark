/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop high-
 * performance, cross-platform applications and libraries. The code contained
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/code/commands.h"


/**
 * Create new files or directories with specific type and permissions.
 * Flags: --type=<file/dir>, --permissions=<rwx>
 */
void shark_create(const char *path, const char *type, const char *permissions) {
    if (type == cnullptr || path == cnullptr) {
        fossil_io_printf("{red}Error: Type and path must be specified.{reset}\n");
        return;
    }

    if (fossil_io_cstring_compare(type, "file") == 0) {
        fossil_fstream_t stream;
        if (fossil_fstream_open(&stream, path, "w") == 0) {
            fossil_fstream_set_permissions(path, strtol(permissions, cnullptr, 8));
            fossil_fstream_close(&stream);
            fossil_io_printf("{green}Created file at %s with permissions %s.{reset}\n", path, permissions);
        } else {
            fossil_io_printf("{red}Error: Failed to create file at %s.{reset}\n", path);
        }
    } else if (fossil_io_cstring_compare(type, "dir") == 0) {
        if (mkdir(path, strtol(permissions, cnullptr, 8)) == 0) {
            fossil_io_printf("{green}Created directory at %s with permissions %s.{reset}\n", path, permissions);
        } else {
            fossil_io_printf("{red}Error: Failed to create directory at %s.{reset}\n", path);
        }
    } else {
        fossil_io_printf("{red}Error: Invalid type specified. Use 'file' or 'dir'.{reset}\n");
    }
}
