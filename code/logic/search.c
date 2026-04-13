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
#include "fossil/code/search.h"

// Helper: find character in string: soon to be added to Fossil Io
static ccstring fossil_io_cstring_find_char(ccstring str, char ch)
{
    if (!str)
        return NULL;
    while (*str)
    {
        if (*str == ch)
            return str;
        str++;
    }
    return NULL;
}

// Helper: detect if file is binary using io_filesys_
static bool is_binary_file(ccstring file_path)
{
    fossil_io_filesys_file_t f;
    if (fossil_io_filesys_file_open(&f, file_path, "rb") != 0)
        return true;

    unsigned char buf[512];
    size_t n = fossil_io_filesys_file_read(&f, buf, 1, sizeof(buf));
    fossil_io_filesys_file_close(&f);

    for (size_t i = 0; i < n; i++)
        if (buf[i] == 0)
            return true;

    return false;
}

// Helper: check if pattern contains regex metacharacters
static bool has_regex_chars(ccstring pattern)
{
    if (!pattern)
        return false;
    const char *metacharacters = ".*+?[](){}|^$\\";
    for (size_t i = 0; pattern[i]; i++)
    {
        if (fossil_io_cstring_find_char(metacharacters, pattern[i]) != NULL)
            return true;
    }
    return false;
}

// Helper: regex-based string match
static bool str_match(ccstring str, fossil_io_regex_t *regex)
{
    if (!str || !regex)
        return !regex;
    return fossil_io_regex_match(regex, str, NULL) > 0;
}

// Helper: check file size filter using io_filesys_
static bool check_file_size(ccstring file_path, uint64_t min_size, uint64_t max_size)
{
    if (!file_path)
        return false;

    int32_t size = fossil_io_filesys_file_size(file_path);
    if (size < 0)
        return false;
    if (min_size > 0 && (uint64_t)size < min_size)
        return false;
    if (max_size > 0 && (uint64_t)size > max_size)
        return false;
    return true;
}

// Helper: search within file contents with context using io_filesys_
static bool content_match(ccstring file_path, fossil_io_regex_t *regex, int *line_num)
{
    if (!regex)
        return true;

    if (is_binary_file(file_path))
        return false;

    fossil_io_filesys_file_t stream;
    if (fossil_io_filesys_file_open(&stream, file_path, "r") != 0)
        return false;

    char *line = (char *)fossil_sys_memory_alloc(4096);
    if (cunlikely(!line))
    {
        fossil_io_filesys_file_close(&stream);
        return false;
    }

    bool found = false;
    int current_line = 0;
    size_t matches = 0;

    // Use fgets on stream.handle (assume FILE*)
    FILE *fp = (FILE *)stream.handle;
    while (fgets(line, 4096, fp))
    {
        current_line++;
        if (fossil_io_regex_match(regex, line, NULL) > 0)
        {
            if (!found)
            {
                *line_num = current_line;
                found = true;
            }
            matches++;
            if (matches >= 100)
                break; // Limit matches to prevent excessive output
        }
    }

    fossil_sys_memory_free(line);
    fossil_io_filesys_file_close(&stream);
    return found;
}

// Directory traversal using io_filesys_
static int search_recursive(ccstring path, bool recursive,
                            fossil_io_regex_t *name_regex, fossil_io_regex_t *content_regex,
                            bool has_content_pattern, uint64_t min_size, uint64_t max_size,
                            bool exclude_hidden)
{
    // Directory listing buffer
    fossil_io_filesys_obj_t entries[256];
    size_t entry_count = 0;
    int32_t rc = fossil_io_filesys_dir_list(path, entries, 256, &entry_count);
    if (rc != 0)
    {
        fossil_io_printf("{red}Error opening directory: %s{normal}\n", path);
        return rc;
    }

    for (size_t i = 0; i < entry_count; ++i)
    {
        fossil_io_filesys_obj_t *entry = &entries[i];
        // Extract just the filename for matching
        char filename[FOSSIL_FILESYS_MAX_PATH];
        if (fossil_io_filesys_basename(entry->path, filename, sizeof(filename)) != 0)
            continue;

        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
            continue;

        if (exclude_hidden && filename[0] == '.')
            continue;

        if (entry->type == FOSSIL_FILESYS_TYPE_DIR)
        {
            if (recursive)
                search_recursive(entry->path, recursive, name_regex, content_regex,
                                 has_content_pattern, min_size, max_size, exclude_hidden);
        }
        else if (entry->type == FOSSIL_FILESYS_TYPE_FILE)
        {
            if (!str_match(filename, name_regex))
                continue;

            if (!check_file_size(entry->path, min_size, max_size))
                continue;

            if (has_content_pattern)
            {
                int line_num = 0;
                if (content_match(entry->path, content_regex, &line_num))
                {
                    fossil_io_printf("{cyan}%s:%d{normal}\n", entry->path, line_num);
                }
            }
            else
            {
                fossil_io_printf("{cyan}%s{normal}\n", entry->path);
            }
        }
    }
    return 0;
}

int fossil_shark_search_advanced(ccstring path, bool recursive,
                                 ccstring name_pattern, ccstring content_pattern,
                                 bool ignore_case, uint64_t min_size, uint64_t max_size,
                                 bool exclude_hidden)
{
    if (!path)
        path = ".";

    const char *options[] = {
        ignore_case ? "icase" : NULL,
        NULL};

    char *error = NULL;
    fossil_io_regex_t *name_regex = NULL;
    fossil_io_regex_t *content_regex = NULL;

    if (name_pattern && has_regex_chars(name_pattern))
    {
        name_regex = fossil_io_regex_compile(name_pattern, ignore_case ? options : NULL, &error);
        if (!name_regex && error)
            fossil_sys_memory_free(error);
    }

    if (content_pattern && has_regex_chars(content_pattern))
    {
        content_regex = fossil_io_regex_compile(content_pattern, ignore_case ? options : NULL, &error);
        if (!content_regex && error)
            fossil_sys_memory_free(error);
    }

    int result = search_recursive(path, recursive, name_regex, content_regex,
                                  content_pattern != NULL, min_size, max_size, exclude_hidden);

    if (name_regex)
        fossil_io_regex_free(name_regex);
    if (content_regex)
        fossil_io_regex_free(content_regex);

    return result;
}

/**
 * Legacy wrapper for compatibility
 */
int fossil_shark_search(ccstring path, bool recursive,
                        ccstring name_pattern, ccstring content_pattern,
                        bool ignore_case)
{
    return fossil_shark_search_advanced(path, recursive, name_pattern, content_pattern,
                                        ignore_case, 0, 0, false);
}
