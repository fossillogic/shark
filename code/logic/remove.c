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
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#endif

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

// Helper: move file to system trash (cross-platform)
static int move_to_trash(ccstring path) {
    char *trash_path = (char*)fossil_sys_memory_alloc(4096);
    if (cunlikely(trash_path == cnull)) {
        fossil_io_printf("{red}Failed to allocate memory for trash path{normal}\n");
        return ENOMEM;
    }
    
    ccstring filename = strrchr(path, '/') ? strrchr(path, '/') + 1 : path;
#ifdef _WIN32
    // Windows: use Recycle Bin path or temp directory
    ccstring home = getenv("USERPROFILE");
    if (home == cnull) home = getenv("TEMP");
    if (home == cnull) home = ".";
    snprintf(trash_path, 4096, "%s\\AppData\\Local\\Trash\\%s", home, filename);
#else
    // Unix-like systems: use XDG trash specification
    ccstring home = getenv("HOME");
    if (home == cnull) home = ".";
    snprintf(trash_path, 4096, "%s/.local/share/Trash/files/%s", home, filename);
#endif

    if (fossil_io_file_rename(path, trash_path) != 0) {
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
#ifdef _WIN32
        WIN32_FIND_DATA find_data;
        HANDLE dir;
        char search_path[4096];
        snprintf(search_path, sizeof(search_path), "%s\\*", path);
        
        dir = FindFirstFile(search_path, &find_data);
        if (dir == INVALID_HANDLE_VALUE) {
            if (force) return 0;
            fossil_io_printf("{red}Error opening directory '%s': %d{normal}\n", path, GetLastError());
            return GetLastError();
        }

        do {
            if (fossil_io_cstring_equals(find_data.cFileName, ".") || 
                fossil_io_cstring_equals(find_data.cFileName, "..")) continue;

            cstring child_path = fossil_io_cstring_format("%s\\%s", path, find_data.cFileName);
#else
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
#endif
            if (cunlikely(child_path == cnull)) {
#ifdef _WIN32
                FindClose(dir);
#else
                closedir(dir);
#endif
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
#ifdef _WIN32
                        FindClose(dir);
#else
                        closedir(dir);
#endif
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
                    if (fossil_io_file_delete(child_path) != 0 && !force) {
                        fossil_io_printf("{red}Failed to remove '%s': %s{normal}\n", child_path, strerror(errno));
                    } else if (!force) {
                        fossil_io_printf("{blue}Removed file: %s{normal}\n", child_path);
                    }
                }
            }
            fossil_io_cstring_free(child_path);
            cnullify(child_path);
#ifdef _WIN32
        } while (FindNextFile(dir, &find_data));
        FindClose(dir);
#else
        }
        closedir(dir);
#endif

        if (interactive && !force) {
            if (!confirm_removal(path)) return 0;
        }

        if (use_trash) return move_to_trash(path);
#ifdef _WIN32
        if (_rmdir(path) != 0 && !force) {
#else
        if (rmdir(path) != 0 && !force) {
#endif
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
        if (fossil_io_file_delete(path) != 0 && !force) {
            fossil_io_printf("{red}Failed to remove '%s': %s{normal}\n", path, strerror(errno));
            return errno;
        } else if (!force) {
            fossil_io_printf("{blue}Removed file: %s{normal}\n", path);
        }
    }

    return 0;
}

int fossil_shark_remove(ccstring path, bool recursive, bool force,
                        bool interactive, bool use_trash) {
    if (cunlikely(path == cnull)) {
        fossil_io_printf("{red}Error: Path must be specified.{normal}\n");
        return 1;
    }

    return remove_recursive(path, recursive, force, interactive, use_trash);
}
