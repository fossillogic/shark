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
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>   // strcasecmp on POSIX
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

/**
 * Helper: determine simple MIME/type from file extension
 */
static ccstring get_mime_type(ccstring path) {
    ccstring ext = strrchr(path, '.');
    if (!cnotnull(ext)) return "application/octet-stream";
    ext++;
    if (fossil_io_cstring_iequals(ext, "txt")) return "text/plain";
    if (fossil_io_cstring_iequals(ext, "c") || fossil_io_cstring_iequals(ext, "h")) return "text/plain";
    if (fossil_io_cstring_iequals(ext, "html")) return "text/html";
    if (fossil_io_cstring_iequals(ext, "json")) return "application/json";
    if (fossil_io_cstring_iequals(ext, "jpg") || fossil_io_cstring_iequals(ext, "jpeg")) return "image/jpeg";
    if (fossil_io_cstring_iequals(ext, "png")) return "image/png";
    if (fossil_io_cstring_iequals(ext, "zip")) return "application/zip";
    return "application/octet-stream";
}

int fossil_shark_introspect(ccstring path, int show_head_lines,
                             int show_tail_lines, bool count_lines_words_bytes,
                             bool show_file_type, bool output_fson)
{
    if (!cnotnull(path)) return -1;

    struct stat st;
    if (stat(path, &st) != 0) return errno;

    fossil_io_file_t file_stream;
    if (fossil_io_file_open(&file_stream, path, "r") != 0 &&
        (show_head_lines > 0 || show_tail_lines > 0 || count_lines_words_bytes)) {
        return errno;
    }

    unsigned long lines = 0, words = 0, bytes = 0;
    char buffer[4096];
    cstring *tail_buffer = cnull;
    int tail_index = 0;

    if (show_tail_lines > 0) {
        tail_buffer = (cstring *)fossil_sys_memory_alloc(sizeof(cstring) * show_tail_lines);
        if (!cnotnull(tail_buffer)) {
            if (fossil_io_file_is_open(&file_stream)) fossil_io_file_close(&file_stream);
            return -1;
        }
        for (int i = 0; i < show_tail_lines; i++)
            tail_buffer[i] = cnull;
    }

    int head_count = 0;
    while (fossil_io_file_is_open(&file_stream) &&
           fgets(buffer, sizeof(buffer), file_stream.file)) {
        size_t len = strlen(buffer);
        bytes += len;
        if (count_lines_words_bytes || show_head_lines > 0 || show_tail_lines > 0) lines++;

        if (count_lines_words_bytes) {
            // Count words roughly
            char *p = buffer;
            bool in_word = false;
            while (*p) {
                if (isspace((unsigned char)*p)) {
                    if (in_word) words++;
                    in_word = false;
                } else {
                    in_word = true;
                }
                p++;
            }
            if (in_word) words++;
        }

        if (show_head_lines > 0 && head_count < show_head_lines) {
            fossil_io_printf("{green,bold}%s{normal}", buffer);
            head_count++;
        }

        if (show_tail_lines > 0) {
            // store last N lines in circular buffer
            if (cnotnull(tail_buffer[tail_index]))
                fossil_io_cstring_free(tail_buffer[tail_index]);
            tail_buffer[tail_index] = fossil_io_cstring_dup(buffer);
            tail_index = (tail_index + 1) % show_tail_lines;
        }
    }

    if (show_tail_lines > 0) {
        fossil_io_printf("\n{yellow,bold}-- Tail last %d lines --{normal}\n", show_tail_lines);
        int start = tail_index;
        for (int i = 0; i < show_tail_lines; i++) {
            int idx = (start + i) % show_tail_lines;
            if (cnotnull(tail_buffer[idx])) fossil_io_printf("{yellow}%s{normal}", tail_buffer[idx]);
        }
    }

    if (fossil_io_file_is_open(&file_stream)) fossil_io_file_close(&file_stream);

    // Output metadata
    if (output_fson) {
        fossil_io_printf("{cyan,bold}{\n");
        fossil_io_printf("  path: cstr: \"%s\",\n", path);
        fossil_io_printf("  size: i64: %lld,\n", (long long)st.st_size);
        fossil_io_printf("  lines: u64: %lu,\n", lines);
        fossil_io_printf("  words: u64: %lu,\n", words);
        fossil_io_printf("  bytes: u64: %lu", bytes);
        if (show_file_type) {
            fossil_io_printf(",\n  type: cstr: \"%s\"", get_mime_type(path));
        }
        fossil_io_printf("\n}{normal}\n");
    } else {
        fossil_io_printf("{cyan,bold}File: %s{normal}\n", path);
        fossil_io_printf("{magenta}Size: %lld bytes{normal}\n", (long long)st.st_size);
        if (count_lines_words_bytes) {
            fossil_io_printf("{blue}Lines: %lu Words: %lu Bytes: %lu{normal}\n", lines, words, bytes);
        }
        if (show_file_type) {
            fossil_io_printf("{green}Type: %s{normal}\n", get_mime_type(path));
        }
    }

    if (cnotnull(tail_buffer)) {
        for (int i = 0; i < show_tail_lines; i++) {
            if (cnotnull(tail_buffer[i]))
                fossil_io_cstring_free(tail_buffer[i]);
        }
        fossil_sys_memory_free(tail_buffer);
        cdrop(tail_buffer);
    }

    return 0;
}
