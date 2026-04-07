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

// Helper: ask user for confirmation
static bool confirm_removal(ccstring path)
{
    char answer[8];
    fossil_io_printf("{cyan}Remove '{bold}%s{normal}{cyan}'? [y/N]: {normal}", path);

    if (fossil_io_gets(answer, sizeof(answer)) != 0)
    {
        return false;
    }

    fossil_io_trim(answer);
    return (answer[0] == 'y' || answer[0] == 'Y');
}

// Helper: write deletion log entry
static void log_deletion(ccstring log_file, ccstring path, bool success)
{
    if (log_file == cnull)
        return;

    fossil_io_filesys_file_t f;
    if (fossil_io_filesys_file_open(&f, log_file, "a") != 0)
        return;

    time_t now = time(cnull);
    char time_str[26];
#ifdef _WIN32
    ctime_s(time_str, sizeof(time_str), &now);
#else
    ctime_r(&now, time_str);
#endif
    time_str[24] = '\0'; // Remove newline

    char buf[1024];
    int n = snprintf(buf, sizeof(buf), "[%s] %s: %s\n", time_str, success ? "OK" : "FAIL", path);
    fossil_io_filesys_file_write(&f, buf, 1, n);

    fossil_io_filesys_file_close(&f);
}

// Helper: securely wipe file before deletion
static int wipe_file(ccstring path, int passes)
{
    fossil_io_filesys_file_t f;
    if (fossil_io_filesys_file_open(&f, path, "r+b") != 0)
        return errno;

    int64_t size = fossil_io_filesys_file_tell(&f);
    if (size < 0) {
        fossil_io_filesys_file_close(&f);
        return errno;
    }
    fossil_io_filesys_file_seek(&f, 0, SEEK_END);
    size = fossil_io_filesys_file_tell(&f);
    fossil_io_filesys_file_seek(&f, 0, SEEK_SET);

    unsigned char *buffer = (unsigned char *)fossil_sys_memory_alloc((size_t)size);
    if (buffer == cnull)
    {
        fossil_io_filesys_file_close(&f);
        return ENOMEM;
    }

    for (int i = 0; i < passes; i++)
    {
        memset(buffer, (i % 256), (size_t)size);
        fossil_io_filesys_file_seek(&f, 0, SEEK_SET);
        fossil_io_filesys_file_write(&f, buffer, 1, (size_t)size);
        fossil_io_filesys_file_flush(&f);
    }

    fossil_sys_memory_free(buffer);
    fossil_io_filesys_file_close(&f);
    return 0;
}

// Helper: move file to system trash (cross-platform)
static int move_to_trash(ccstring path)
{
    char *trash_path = (char *)fossil_sys_memory_alloc(4096);
    if (cunlikely(trash_path == cnull))
    {
        fossil_io_printf("{red}Failed to allocate memory for trash path{normal}\n");
        return ENOMEM;
    }

    char filename[FOSSIL_FILESYS_MAX_PATH];
    fossil_io_filesys_basename(path, filename, sizeof(filename));
#ifdef _WIN32
    ccstring home = getenv("USERPROFILE");
    if (home == cnull)
        home = getenv("TEMP");
    if (home == cnull)
        home = ".";
    snprintf(trash_path, 4096, "%s\\AppData\\Local\\Trash\\%s", home, filename);
#else
    ccstring home = getenv("HOME");
    if (home == cnull)
        home = ".";
    snprintf(trash_path, 4096, "%s/.local/share/Trash/files/%s", home, filename);
#endif

    if (fossil_io_filesys_move(path, trash_path) != 0)
    {
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

// Helper: check if file matches criteria
static bool matches_criteria(ccstring path, ccstring older_than, size_t larger_than)
{
    fossil_io_filesys_obj_t obj;
    if (fossil_io_filesys_stat(path, &obj) != 0)
        return false;

    if (larger_than > 0 && obj.size <= larger_than)
        return false;

    if (older_than != cnull)
    {
        time_t cutoff = atol(older_than);
        if (obj.modified_at > cutoff)
            return false;
    }

    return true;
}

// Internal recursive removal using fossil_io_filesys_*
static int remove_recursive(ccstring path, bool recursive, bool force,
                            bool interactive, bool use_trash, bool wipe,
                            int shred_passes, ccstring older_than,
                            size_t larger_than, bool empty_only,
                            ccstring log_file)
{
    if (cunlikely(path == cnull))
    {
        fossil_io_printf("{red}Error: Path cannot be null{normal}\n");
        return EINVAL;
    }

    fossil_io_filesys_obj_t obj;
    if (fossil_io_filesys_stat(path, &obj) != 0)
    {
        if (force)
            return 0;
        fossil_io_printf("{red}Error accessing '%s': %s{normal}\n", path, strerror(errno));
        return errno;
    }

    if (obj.type == FOSSIL_FILESYS_TYPE_DIR)
    {
        fossil_io_filesys_obj_t entries[256];
        size_t count = 0;
        if (fossil_io_filesys_dir_list(path, entries, 256, &count) != 0)
        {
            if (force)
                return 0;
            fossil_io_printf("{red}Error opening directory '%s': %s{normal}\n", path, strerror(errno));
            return errno;
        }

        for (size_t i = 0; i < count; ++i)
        {
            fossil_io_filesys_obj_t *entry = &entries[i];
            if (strcmp(entry->path, ".") == 0 || strcmp(entry->path, "..") == 0)
                continue;

            if (entry->type == FOSSIL_FILESYS_TYPE_DIR)
            {
                if (recursive)
                {
                    if (remove_recursive(entry->path, recursive, force, interactive, use_trash, wipe, shred_passes, older_than, larger_than, empty_only, log_file) != 0 && !force)
                    {
                        return 1;
                    }
                }
                else
                {
                    if (!force)
                        fossil_io_printf("{red}Cannot remove directory '%s' without recursive flag.{normal}\n", entry->path);
                    continue;
                }
            }
            else
            {
                if (!matches_criteria(entry->path, older_than, larger_than))
                    continue;

                if (interactive && !force)
                {
                    if (!confirm_removal(entry->path))
                    {
                        continue;
                    }
                }

                if (wipe && shred_passes > 0)
                {
                    wipe_file(entry->path, shred_passes);
                }

                if (use_trash)
                {
                    if (move_to_trash(entry->path) != 0 && !force)
                    {
                        log_deletion(log_file, entry->path, false);
                        continue;
                    }
                }
                else
                {
                    if (fossil_io_filesys_remove(entry->path, false) != 0 && !force)
                    {
                        fossil_io_printf("{red}Failed to remove '%s': %s{normal}\n", entry->path, strerror(errno));
                        log_deletion(log_file, entry->path, false);
                    }
                    else
                    {
                        fossil_io_printf("{blue}Removed file: %s{normal}\n", entry->path);
                        log_deletion(log_file, entry->path, true);
                    }
                }
            }
        }

        if (empty_only)
        {
            fossil_io_filesys_obj_t dir_obj;
            if (fossil_io_filesys_stat(path, &dir_obj) == 0 && dir_obj.size > 0)
                return 0;
        }

        if (interactive && !force)
        {
            if (!confirm_removal(path))
                return 0;
        }

        if (use_trash)
            return move_to_trash(path);
        if (fossil_io_filesys_remove(path, false) != 0 && !force)
        {
            fossil_io_printf("{red}Failed to remove directory '%s': %s{normal}\n", path, strerror(errno));
            log_deletion(log_file, path, false);
            return errno;
        }
        else
        {
            fossil_io_printf("{blue}Removed directory: %s{normal}\n", path);
            log_deletion(log_file, path, true);
        }
    }
    else
    {
        if (!matches_criteria(path, older_than, larger_than))
            return 0;

        if (interactive && !force)
        {
            if (!confirm_removal(path))
                return 0;
        }

        if (wipe && shred_passes > 0)
        {
            wipe_file(path, shred_passes);
        }

        if (use_trash)
            return move_to_trash(path);
        if (fossil_io_filesys_remove(path, false) != 0 && !force)
        {
            fossil_io_printf("{red}Failed to remove '%s': %s{normal}\n", path, strerror(errno));
            log_deletion(log_file, path, false);
            return errno;
        }
        else
        {
            fossil_io_printf("{blue}Removed file: %s{normal}\n", path);
            log_deletion(log_file, path, true);
        }
    }

    return 0;
}

int fossil_spino_remove(ccstring path, bool recursive, bool force,
                        bool interactive, bool use_trash, bool wipe,
                        int shred_passes, ccstring older_than,
                        size_t larger_than, bool empty_only,
                        ccstring log_file)
{
    if (cunlikely(path == cnull))
    {
        fossil_io_printf("{red}Error: Path must be specified.{normal}\n");
        return 1;
    }

    return remove_recursive(path, recursive, force, interactive, use_trash, wipe, shred_passes, older_than, larger_than, empty_only, log_file);
}
