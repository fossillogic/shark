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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// Helper: ask user for confirmation
static bool confirm_overwrite_rename(ccstring path) {
    char *answer = (char *)fossil_sys_memory_alloc(8);
    fossil_io_printf("{cyan}Overwrite '%s'? [y/N]: {normal}", path);
    
    if (fossil_io_gets(answer, 8) != 0) {
        fossil_sys_memory_free(answer);
        return false;
    }
    
    fossil_io_trim(answer);
    bool result = (answer[0] == 'y' || answer[0] == 'Y');
    fossil_sys_memory_free(answer);
    return result;
}

/**
 * Rename files or directories with safety checks
 */
int fossil_shark_rename(ccstring old_name, ccstring new_name,
                        bool force, bool interactive) {
    if (cunlikely(!old_name || !new_name)) {
        fossil_io_printf("{red}Error: Old and new names must be specified.{normal}\n");
        return 1;
    }

    // Use safe null checking
    ccstring safe_old = cunwrap_or(old_name, cempty);
    ccstring safe_new = cunwrap_or(new_name, cempty);
    
    if (cunlikely(safe_old == cempty || safe_new == cempty)) {
        fossil_io_printf("{red}Error: Invalid file names provided.{normal}\n");
        return 1;
    }

    bool dest_exists = fossil_fstream_file_exists(safe_new);

    if (clikely(dest_exists)) {
        if (interactive && !force) {
            if (!confirm_overwrite_rename(safe_new)) {
                fossil_io_printf("{blue}Rename cancelled by user.{normal}\n");
                return 1;
            }
        }

        if (cunlikely(!force && !interactive)) {
            fossil_io_printf("{red}Error: Destination exists. Use --force or --interactive to overwrite.{normal}\n");
            return 1;
        }

        // Remove destination before renaming if force is specified
        if (force) {
            if (cunlikely(fossil_fstream_delete(safe_new) != 0)) {
                fossil_io_printf("{red}Error: Failed to remove existing file/directory{normal}\n");
                return 1;
            }
        }
    }

    if (cunlikely(fossil_fstream_rename(safe_old, safe_new) != 0)) {
        fossil_io_printf("{red}Error: Rename failed{normal}\n");
        return 1;
    }

    fossil_io_printf("{cyan}Successfully renamed '%s' to '%s'{normal}\n", safe_old, safe_new);
    return 0;
}
