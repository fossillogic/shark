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
#include <errno.h>

static int create_backup(ccstring dest) {
    cstring backup_path = fossil_io_cstring_format("%s.bak", dest);
    if (!cnotnull(backup_path)) {
        fossil_io_printf("{red}Failed to create backup path{normal}\n");
        return -1;
    }
    
    if (fossil_fstream_backup(dest, ".bak") != 0) {
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

    bool dest_exists = fossil_fstream_file_exists(dest);

    if (clikely(dest_exists)) {
        if (backup) {
            if (create_backup(dest) != 0) return 1;
        }

        if (interactive && !force) {
            if (!confirm_overwrite(dest)) {
                fossil_io_printf("{cyan}Move cancelled by user.{normal}\n");
                return 1;
            }
        }

        if (cunlikely(!force && !interactive && !backup)) {
            fossil_io_printf("{red}Error: Destination exists. Use --force, --interactive, or --backup.{normal}\n");
            return 1;
        }
    }

    if (fossil_fstream_rename(src, dest) != 0) {
        fossil_io_printf("{red}Failed to move/rename: %s{normal}\n", strerror(errno));
        return errno;
    }

    fossil_io_printf("{cyan}Successfully moved '%s' to '%s'{normal}\n", src, dest);
    return 0;
}
