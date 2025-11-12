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
#include <dirent.h>
#include <errno.h>

// Helper: ask user for confirmation
static bool confirm_removal(ccstring path) {
    char answer[8];
    fossil_io_printf("{cyan}Remove '{bold}%s{normal}{cyan}'? [y/N]: {normal}", path);
    
    if (fossil_io_gets(answer, sizeof(answer)) != 0) {
        return false;
    }
    
    fossil_io_trim(answer);
    return (answer[0] == 'y' || answer[0] == 'Y');
}

// Helper: move file to system trash (simple cross-platform placeholder)
static int move_to_trash(ccstring path) {
    // On Unix systems, could move to ~/.local/share/Trash/files/
    char *trash_path = (char*)fossil_sys_memory_alloc(4096);
    if (cunlikely(trash_path == cnull)) {
        fossil_io_printf("{red}Failed to allocate memory for trash path{normal}\n");
        return ENOMEM;
    }
    
    ccstring home = getenv("HOME");
    if (home == cnull) home = ".";
    
    ccstring filename = strrchr(path, '/') ? strrchr(path, '/') + 1 : path;
    snprintf(trash_path, 4096, "%s/.local/share/Trash/files/%s", home, filename);

    if (fossil_fstream_rename(path, trash_path) != 0) {
        fossil_io_printf("{red}Failed to move to trash: %s{normal}\n", strerror(errno));
        fossil_sys_memory_free(trash_path);
        cnullify(trash_path);
        return errno;
    }
    fossil_io_printf("{cyan}Moved '{bold}%s{normal}{cyan}' to trash.{normal}\n", path);
    fossil_sys_memory_free(trash_path);
    cnullify(trash_path);
    return 0;
}

// Internal recursive removal
static int remove_recursive(ccstring path, bool recursive, bool force,
                            bool interactive, bool use_trash) {
    if (cunlikely(path == cnull)) {
        fossil_io_printf("{red}Error: Path cannot be null{normal}\n");
        return EINVAL;
    }

    struct stat st;
    if (stat(path, &st) != 0) {
        if (force) return 0;
        fossil_io_printf("{red}Error accessing '%s': %s{normal}\n", path, strerror(errno));
        return errno;
    }

    if (S_ISDIR(st.st_mode)) {
        DIR *dir = opendir(path);
        if (dir == cnull) {
            if (force) return 0;
            fossil_io_printf("{red}Error opening directory '%s': %s{normal}\n", path, strerror(errno));
            return errno;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != cnull) {
            if (fossil_io_cstring_equals(entry->d_name, ".") || fossil_io_cstring_equals(entry->d_name, "..")) continue;

            cstring child_path = fossil_io_cstring_format("%s/%s", path, entry->d_name);
            if (cunlikely(child_path == cnull)) {
                closedir(dir);
                return ENOMEM;
            }

            struct stat child_st;
            if (stat(child_path, &child_st) != 0) {
                if (!force) fossil_io_printf("{red}Error accessing '%s': %s{normal}\n", child_path, strerror(errno));
                fossil_io_cstring_free(child_path);
                cnullify(child_path);
                continue;
            }

            if (S_ISDIR(child_st.st_mode)) {
                if (recursive) {
                    if (remove_recursive(child_path, recursive, force, interactive, use_trash) != 0 && !force) {
                        fossil_io_cstring_free(child_path);
                        cnullify(child_path);
                        closedir(dir);
                        return 1;
                    }
                } else {
                    if (!force) fossil_io_printf("{red}Cannot remove directory '%s' without recursive flag.{normal}\n", child_path);
                    fossil_io_cstring_free(child_path);
                    cnullify(child_path);
                    continue;
                }
            } else {
                if (interactive && !force) {
                    if (!confirm_removal(child_path)) {
                        fossil_io_cstring_free(child_path);
                        cnullify(child_path);
                        continue;
                    }
                }

                if (use_trash) {
                    if (move_to_trash(child_path) != 0 && !force) {
                        fossil_io_cstring_free(child_path);
                        cnullify(child_path);
                        continue;
                    }
                } else {
                    if (fossil_fstream_delete(child_path) != 0 && !force) {
                        fossil_io_printf("{red}Failed to remove '%s': %s{normal}\n", child_path, strerror(errno));
                    } else if (!force) {
                        fossil_io_printf("{blue}Removed file: %s{normal}\n", child_path);
                    }
                }
            }
            fossil_io_cstring_free(child_path);
            cnullify(child_path);
        }

        closedir(dir);

        if (interactive && !force) {
            if (!confirm_removal(path)) return 0;
        }

        if (use_trash) return move_to_trash(path);
        if (rmdir(path) != 0 && !force) {
            fossil_io_printf("{red}Failed to remove directory '%s': %s{normal}\n", path, strerror(errno));
            return errno;
        } else if (!force) {
            fossil_io_printf("{blue}Removed directory: %s{normal}\n", path);
        }
    } else {
        if (interactive && !force) {
            if (!confirm_removal(path)) return 0;
        }
        if (use_trash) return move_to_trash(path);
        if (fossil_fstream_delete(path) != 0 && !force) {
            fossil_io_printf("{red}Failed to remove '%s': %s{normal}\n", path, strerror(errno));
            return errno;
        } else if (!force) {
            fossil_io_printf("{blue}Removed file: %s{normal}\n", path);
        }
    }

    return 0;
}

/**
 * Remove or delete files and directories safely
 */
int fossil_shark_remove(ccstring path, bool recursive, bool force,
                        bool interactive, bool use_trash) {
    if (cunlikely(path == cnull)) {
        fossil_io_printf("{red}Error: Path must be specified.{normal}\n");
        return 1;
    }

    return remove_recursive(path, recursive, force, interactive, use_trash);
}
