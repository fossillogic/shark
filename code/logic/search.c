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
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#endif

// Helper: case-insensitive string match using fossil_io_cstring functions
static bool str_match(ccstring str, ccstring pattern, bool ignore_case) {
    if (!pattern) return true; // No pattern means match all
    
    if (!ignore_case) {
        return fossil_io_cstring_contains(str, pattern);
    } else {
        return fossil_io_cstring_icontains(str, pattern);
    }
}

// Helper: search within file contents
static bool content_match(ccstring file_path, ccstring pattern, bool ignore_case) {
    if (!pattern) return true; // No pattern means match all

    fossil_io_file_t stream;
    if (fossil_io_file_open(&stream, file_path, "r") != 0) {
        return false;
    }

    char *line = (char*)fossil_sys_memory_alloc(4096);
    if (cunlikely(!line)) {
        fossil_io_file_close(&stream);
        return false;
    }
    
    bool found = false;
    while (fossil_io_gets_from_stream(line, 4096, &stream)) {
        fossil_io_trim(line); // Trim whitespace from line
        if (str_match(line, pattern, ignore_case)) {
            found = true;
            break;
        }
    }

    fossil_sys_memory_free(line);
    fossil_io_file_close(&stream);
    return found;
}

#ifdef _WIN32
// Windows implementation using FindFirstFile/FindNextFile
static int search_recursive(ccstring path, bool recursive,
                            ccstring name_pattern, ccstring content_pattern,
                            bool ignore_case) {
    cstring search_path = fossil_io_cstring_format("%s\\*", path);
    if (!search_path) return -1;

    WIN32_FIND_DATAA find_data;
    HANDLE hFind = FindFirstFileA(search_path, &find_data);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        fossil_io_cstring_free(search_path);
        return GetLastError();
    }

    do {
        if (fossil_io_cstring_equals(find_data.cFileName, ".") || 
            fossil_io_cstring_equals(find_data.cFileName, "..")) continue;

        cstring full_path = fossil_io_cstring_format("%s\\%s", path, find_data.cFileName);
        if (!full_path) continue;

        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (recursive) search_recursive(full_path, recursive, name_pattern, content_pattern, ignore_case);
        } else {
            if (str_match(find_data.cFileName, name_pattern, ignore_case) &&
                content_match(full_path, content_pattern, ignore_case)) {
                fossil_io_printf("{cyan}%s{normal}\n", full_path);
            }
        }
        
        fossil_io_cstring_free(full_path);
    } while (FindNextFileA(hFind, &find_data) != 0);

    FindClose(hFind);
    fossil_io_cstring_free(search_path);
    return 0;
}
#else
// Unix/Linux implementation using opendir/readdir
static int search_recursive(ccstring path, bool recursive,
                            ccstring name_pattern, ccstring content_pattern,
                            bool ignore_case) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror(path);
        return errno;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (fossil_io_cstring_equals(entry->d_name, ".") || 
            fossil_io_cstring_equals(entry->d_name, "..")) continue;

        cstring full_path = fossil_io_cstring_format("%s/%s", path, entry->d_name);
        if (cunlikely(!full_path)) continue;

        struct stat *st = (struct stat*)fossil_sys_memory_alloc(sizeof(struct stat));
        if (cunlikely(!st)) {
            fossil_io_cstring_free(full_path);
            continue;
        }
        
        if (stat(full_path, st) != 0) {
            fossil_io_cstring_free(full_path);
            fossil_sys_memory_free(st);
            continue;
        }

        if (S_ISDIR(st->st_mode)) {
            if (recursive) search_recursive(full_path, recursive, name_pattern, content_pattern, ignore_case);
        } else if (S_ISREG(st->st_mode)) {
            if (str_match(entry->d_name, name_pattern, ignore_case) &&
                content_match(full_path, content_pattern, ignore_case)) {
                fossil_io_printf("{cyan}%s{normal}\n", full_path);
            }
        }
        
        fossil_io_cstring_free(full_path);
        fossil_sys_memory_free(st);
    }

    closedir(dir);
    return 0;
}
#endif

/**
 * Search for files by name patterns or content matching
 */
int fossil_shark_search(ccstring path, bool recursive,
                        ccstring name_pattern, ccstring content_pattern,
                        bool ignore_case) {
    if (!path) path = ".";
    return search_recursive(path, recursive, name_pattern, content_pattern, ignore_case);
}
