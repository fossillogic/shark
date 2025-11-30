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

#define INDENT_SIZE 4


static void print_size(off_t size, bool human_readable) {
    if (!human_readable) {
        fossil_io_printf("{cyan}%lld{normal} ", (long long)size);
        return;
    }
    ccstring units[] = {"B", "KB", "MB", "GB", "TB"};
    double sz = (double)size;
    int i = 0;
    while (sz >= 1024 && i < 4) { sz /= 1024; i++; }
    fossil_io_printf("{blue,underline}%.1f%s{normal} ", sz, units[i]);
}

static void print_permissions_advanced(const char *filename) {
#ifdef _WIN32
    fossil_io_printf("{yellow,bold}%c{normal}", fossil_io_file_is_readable(filename) ? '-' : '-');
    fossil_io_printf("{green}%c{normal}", fossil_io_file_is_readable(filename) ? 'r' : '-');
    fossil_io_printf("{green}%c{normal}", fossil_io_file_is_writable(filename) ? 'w' : '-');
    fossil_io_printf("{green}%c{normal} ", fossil_io_file_is_executable(filename) ? 'x' : '-');
#else
    fossil_io_printf("{yellow,bold}%c{normal}", '-'); // Directory/file type not available via API
    // User
    fossil_io_printf("{green}%c{normal}", fossil_io_file_is_readable(filename) ? 'r' : '-');
    fossil_io_printf("{green}%c{normal}", fossil_io_file_is_writable(filename) ? 'w' : '-');
    fossil_io_printf("{green}%c{normal}", fossil_io_file_is_executable(filename) ? 'x' : '-');
    // Group and Other not available via API, print as '-'
    fossil_io_printf("{magenta}---{normal}");
    fossil_io_printf("{cyan}---{normal} ");
#endif
}

static int show_list(ccstring path, bool show_all, bool long_format,
                     bool human_readable, bool show_time) {
    fossil_io_dir_iter_t it;
    int32_t open_result = fossil_io_dir_iter_open(&it, path);
    if (open_result < 0) return open_result;

    fossil_io_printf("{pos:top}{bold,underline,blue}Directory Listing: %s{normal}\n", path);

    while (fossil_io_dir_iter_next(&it) > 0 && it.active) {
        fossil_io_dir_entry_t *entry = &it.current;
        if (!show_all && entry->name[0] == '.') continue;

        if (long_format) {
            print_permissions_advanced(entry->path);
            print_size(entry->size, human_readable);
            if (show_time) {
                fossil_io_printf("{bright_black}%llu{normal} ", (unsigned long long)entry->modified);
            }
        }
        fossil_io_printf("{green}%s{normal}\n", entry->name);
    }

    fossil_io_dir_iter_close(&it);
    fossil_io_flush();
    return 0;
}

static int show_tree(ccstring path, bool show_all, bool long_format,
                     bool human_readable, bool show_time,
                     int depth, int current_depth, ccstring prefix)
{
    fossil_io_dir_iter_t it;
    int32_t open_result = fossil_io_dir_iter_open(&it, path);
    if (open_result < 0) return open_result;

    if (current_depth == 0) {
        fossil_io_printf("{pos:top}{bold,underline,blue}Directory Tree: %s{normal}\n", path);
    }

    while (fossil_io_dir_iter_next(&it) > 0 && it.active) {
        fossil_io_dir_entry_t *entry = &it.current;
        if (!show_all && entry->name[0] == '.') continue;

        fossil_io_printf("%s", prefix ? prefix : "");
        fossil_io_printf("{bright_yellow}|--{normal} ");

        if (long_format) {
            print_permissions_advanced(entry->path);
            print_size(entry->size, human_readable);
            if (show_time) {
                fossil_io_printf("{bright_black}%llu{normal} ", (unsigned long long)entry->modified);
            }
        }
        fossil_io_printf("{cyan}%s{normal}\n", entry->name);

        if (entry->type == 1 &&
            strcmp(entry->name, ".") != 0 &&
            strcmp(entry->name, "..") != 0 &&
            (depth == 0 || current_depth < depth)) {
            cstring new_prefix = fossil_io_cstring_format("%s%s", prefix ? prefix : "", "    ");
            show_tree(entry->path, show_all, long_format, human_readable, show_time,
                      depth, current_depth + 1, new_prefix);
            fossil_io_cstring_free(new_prefix);
            cnullify(new_prefix);
        }
    }

    fossil_io_dir_iter_close(&it);
    fossil_io_flush();
    return 0;
}

static int show_graph(ccstring path, bool show_all, bool long_format,
                      bool human_readable, bool show_time,
                      int depth, int current_depth, int indent)
{
    fossil_io_dir_iter_t it;
    int32_t open_result = fossil_io_dir_iter_open(&it, path);
    if (open_result < 0) return open_result;

    if (current_depth == 0) {
        fossil_io_printf("{pos:top}{bold,underline,blue}Directory Graph: %s{normal}\n", path);
    }

    while (fossil_io_dir_iter_next(&it) > 0 && it.active) {
        fossil_io_dir_entry_t *entry = &it.current;
        if (!show_all && entry->name[0] == '.') continue;

        fossil_io_move_cursor(current_depth + 2, indent * INDENT_SIZE + 1);
        fossil_io_printf("{bright_yellow}|--{normal} ");

        if (long_format) {
            print_permissions_advanced(entry->path);
            print_size(entry->size, human_readable);
            if (show_time) {
                fossil_io_printf("{bright_black}%llu{normal} ", (unsigned long long)entry->modified);
            }
        }
        fossil_io_printf("{magenta}%s{normal}\n", entry->name);

        if (entry->type == 1 &&
            strcmp(entry->name, ".") != 0 &&
            strcmp(entry->name, "..") != 0 &&
            (depth == 0 || current_depth < depth)) {
            show_graph(entry->path, show_all, long_format, human_readable, show_time,
                       depth, current_depth + 1, indent + 1);
        }
    }

    fossil_io_dir_iter_close(&it);
    fossil_io_flush();
    return 0;
}

int fossil_shark_show(ccstring path, bool show_all, bool long_format,
                      bool human_readable, bool recursive,
                      ccstring format, bool show_time, int depth) {
    if (cunlikely(!path)) path = ".";

    char *sanitized_path = fossil_sys_memory_alloc(1024);
    if (cunlikely(!sanitized_path)) {
        return ENOMEM;
    }
    
    int sanitize_result = fossil_io_validate_sanitize_string(path, sanitized_path, 
                                                            1024, 
                                                            FOSSIL_CTX_FILENAME);
    if (sanitize_result & (FOSSIL_SAN_PATH | FOSSIL_SAN_SCRIPT | FOSSIL_SAN_SHELL)) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Suspicious path detected, using sanitized version{normal}\n");
        path = sanitized_path;
    }

    int effective_depth = recursive ? depth : 1;

    fossil_io_clear_screen();

    int result = 0;
    if (cunlikely(!format) || fossil_io_cstring_equals(format, "list")) {
        result = show_list(path, show_all, long_format, human_readable, show_time);
    } else if (fossil_io_cstring_equals(format, "tree")) {
        result = show_tree(path, show_all, long_format, human_readable, show_time, effective_depth, 0, cempty);
    } else if (fossil_io_cstring_equals(format, "graph")) {
        result = show_graph(path, show_all, long_format, human_readable, show_time, effective_depth, 0, 0);
    } else {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Unknown format: %s{normal}\n", format);
        result = 1;
    }

    fossil_sys_memory_free(sanitized_path);
    cnullify(sanitized_path);
    return result;
}
