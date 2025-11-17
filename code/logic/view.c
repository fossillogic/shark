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
#include <time.h>

/**
 * View and display file contents with formatting options
 * Uses markup tags for colorizing tokens: {red}, {green}, {yellow}, {blue}, {magenta}, {cyan}, {normal}, etc.
 */
int fossil_shark_view(ccstring path, bool number_lines,
                      bool number_non_blank, bool squeeze_blank,
                      int head_lines, int tail_lines, bool show_time) {
    if (path == cnull) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red}Error:{normal} File path must be specified.\n");
        return 1;
    }

    // Determine file type by extension
    bool is_media = false, is_code = false, is_structured = false;
    const char *ext = strrchr(path, '.');
    if (ext) {
        // Media file extensions
        if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0 ||
            strcmp(ext, ".png") == 0 || strcmp(ext, ".gif") == 0 ||
            strcmp(ext, ".mp3") == 0 || strcmp(ext, ".mp4") == 0 ||
            strcmp(ext, ".wav") == 0 || strcmp(ext, ".avi") == 0 ||
            strcmp(ext, ".mov") == 0 || strcmp(ext, ".flac") == 0) {
            is_media = true;
        }
        // Structured data file extensions
        else if (strcmp(ext, ".json") == 0 || strcmp(ext, ".xml") == 0 ||
                 strcmp(ext, ".yaml") == 0 || strcmp(ext, ".yml") == 0 ||
                 strcmp(ext, ".toml") == 0 || strcmp(ext, ".ini") == 0 ||
                 strcmp(ext, ".csv") == 0) {
            is_structured = true;
        }
        // Code file extensions
        else if (strcmp(ext, ".c") == 0 || strcmp(ext, ".cpp") == 0 ||
                 strcmp(ext, ".h") == 0 || strcmp(ext, ".hpp") == 0 ||
                 strcmp(ext, ".py") == 0 || strcmp(ext, ".js") == 0 ||
                 strcmp(ext, ".java") == 0 || strcmp(ext, ".cs") == 0 ||
                 strcmp(ext, ".go") == 0 || strcmp(ext, ".rs") == 0 ||
                 strcmp(ext, ".sh") == 0 || strcmp(ext, ".html") == 0 ||
                 strcmp(ext, ".css") == 0) {
            is_code = true;
        }
    }

    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, path, "r") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red}Error:{normal} ");
        perror(path);
        return 1;
    }

    // Optionally display timestamps (cross-platform)
    if (show_time) {
#ifdef _WIN32
        struct _stat st;
        if (_stat(path, &st) == 0) {
            fossil_io_printf("{blue}File:{normal} %s\n", path);
            fossil_io_printf("{blue}Size:{normal} %ld bytes\n", (long)st.st_size);
            char time_buf[64];
            struct tm *tm_info = localtime(&st.st_mtime);
            if (tm_info) {
                strftime(time_buf, sizeof(time_buf), "%c", tm_info);
                fossil_io_printf("{blue}Modified:{normal} %s\n", time_buf);
            }
        }
#else
        struct stat st;
        if (stat(path, &st) == 0) {
            fossil_io_printf("{blue}File:{normal} %s\n", path);
            fossil_io_printf("{blue}Size:{normal} %ld bytes\n", (long)st.st_size);
            fossil_io_printf("{blue}Modified:{normal} %s", ctime(&st.st_mtime));
        }
#endif
    }

    // Read all lines into memory if tail_lines is requested
    cstring *lines = cnull;
    size_t count = 0, capacity = 0;
    char buffer[8192];

    while (fossil_io_gets_from_stream(buffer, sizeof(buffer), &stream)) {
        fossil_io_trim(buffer);

        if (squeeze_blank && strlen(buffer) == 0) {
            if (count > 0 && cnotnull(lines) && strlen(lines[count-1]) == 0) continue;
        }

        // Colorize token chars in buffer based on file type using markup tags
        char colored_buf[8192 * 2];
        size_t j = 0;
        for (size_t i = 0; buffer[i] != '\0' && j < sizeof(colored_buf) - 16; ++i) {
            char c = buffer[i];
            if (is_media) {
                // For media files, color everything magenta
                j += snprintf(colored_buf + j, sizeof(colored_buf) - j, "{magenta}%c{normal}", c);
            } else if (is_structured) {
                // For structured data files, color punctuation cyan, keys green, values yellow
                if (c == '{' || c == '}' || c == '[' || c == ']' ||
                    c == ':' || c == ',' || c == '"' || c == '\'') {
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j, "{cyan}%c{normal}", c);
                } else if ((i == 0 || buffer[i-1] == '"' || buffer[i-1] == '\'') && (c >= 'a' && c <= 'z')) {
                    // Simple heuristic: keys (start after quote)
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j, "{green}%c{normal}", c);
                } else if (c >= '0' && c <= '9') {
                    // Values (numbers)
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j, "{yellow}%c{normal}", c);
                } else {
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j, "{normal}%c", c);
                }
            } else if (is_code) {
                // For code files, color token chars yellow, keywords blue, comments green
                if (c == '{' || c == '}' || c == '(' || c == ')' ||
                    c == '[' || c == ']' || c == ';' || c == ',' ||
                    c == '=' || c == '+' || c == '-' || c == '*' ||
                    c == '/' || c == '%' || c == '<' || c == '>' ||
                    c == '&' || c == '|' || c == '^' || c == '!') {
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j, "{yellow}%c{normal}", c);
                } else {
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j, "{normal}%c", c);
                }
            } else {
                // Default: color as green
                j += snprintf(colored_buf + j, sizeof(colored_buf) - j, "{green}%c{normal}", c);
            }
        }
        colored_buf[j] = '\0';

        if (tail_lines > 0) {
            if (count >= capacity) {
                capacity = capacity ? capacity * 2 : 128;
                cstring *new_lines;
                if (lines == cnull) {
                    new_lines = (cstring *)fossil_sys_memory_alloc(capacity * sizeof(cstring));
                } else {
                    new_lines = (cstring *)fossil_sys_memory_realloc(lines, capacity * sizeof(cstring));
                }
                if (new_lines == cnull) {
                    cpanic("Memory allocation failed for lines buffer");
                }
                lines = new_lines;
            }
            lines[count++] = fossil_io_cstring_dup_safe(colored_buf, strlen(colored_buf));
        } else {
            if (head_lines == 0 || (int)count < head_lines) {
                if (number_lines)
                    fossil_io_printf("{blue}%6zu{normal}\t%s\n", count + 1, colored_buf);
                else if (number_non_blank && strlen(buffer) > 0)
                    fossil_io_printf("{blue}%6zu{normal}\t%s\n", count + 1, colored_buf);
                else
                    fossil_io_printf("%s\n", colored_buf);
            }
            count++;
            if (head_lines > 0 && (int)count >= head_lines) break;
        }
    }
    fossil_fstream_close(&stream);

    if (tail_lines > 0 && count > 0 && cnotnull(lines)) {
        size_t start = count > (size_t)tail_lines ? count - tail_lines : 0;
        for (size_t i = start; i < count; i++) {
            if (number_lines)
                fossil_io_printf("{blue}%6zu{normal}\t%s\n", i + 1, lines[i]);
            else if (number_non_blank && strlen(lines[i]) > 0)
                fossil_io_printf("{blue}%6zu{normal}\t%s\n", i + 1, lines[i]);
            else
                fossil_io_printf("%s\n", lines[i]);
            fossil_io_cstring_free_safe(&lines[i]);
        }
        fossil_sys_memory_free(lines);
        cnullify(lines);
    }

    return 0;
}
