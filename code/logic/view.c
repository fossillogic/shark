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
 */
int fossil_shark_view(ccstring path, bool number_lines,
                      bool number_non_blank, bool squeeze_blank,
                      int head_lines, int tail_lines, bool show_time) {
    if (path == cnull) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red}Error:{normal} File path must be specified.\n");
        return 1;
    }

    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, path, "r") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red}Error:{normal} ");
        perror(path);
        return 1;
    }

    // Optionally display timestamps
    if (show_time) {
        struct stat st;
        if (stat(path, &st) == 0) {
            fossil_io_printf("{blue}File:{normal} %s\n", path);
            fossil_io_printf("{blue}Size:{normal} %ld bytes\n", st.st_size);
            fossil_io_printf("{blue}Modified:{normal} %s", ctime(&st.st_mtime));
        }
    }

    // Read all lines into memory if tail_lines is requested
    cstring *lines = cnull;
    size_t count = 0, capacity = 0;
    char buffer[8192];
    
    while (fossil_io_gets_from_stream(buffer, sizeof(buffer), &stream)) {
        // Trim whitespace for better handling
        fossil_io_trim(buffer);
        
        if (squeeze_blank && strlen(buffer) == 0) {
            if (count > 0 && cnotnull(lines) && strlen(lines[count-1]) == 0) continue;
        }

        if (tail_lines > 0) {
            if (count >= capacity) {
                capacity = capacity ? capacity * 2 : 128;
                cstring *new_lines = (cstring *)fossil_sys_memory_realloc(lines, capacity * sizeof(cstring));
                if (new_lines == cnull) {
                    cpanic("Memory allocation failed for lines buffer");
                }
                lines = new_lines;
            }
            lines[count++] = fossil_io_cstring_dup_safe(buffer, strlen(buffer));
        } else {
            // Print immediately for head_lines == 0 or counting from top
            if (head_lines == 0 || (int)count < head_lines) {
                if (number_lines) fossil_io_printf("{blue}%6zu{normal}\t%s\n", count + 1, buffer);
                else if (number_non_blank && strlen(buffer) > 0)
                    fossil_io_printf("{blue}%6zu{normal}\t%s\n", count + 1, buffer);
                else
                    fossil_io_printf("%s\n", buffer);
            }
            count++;
            if (head_lines > 0 && (int)count >= head_lines) break;
        }
    }
    fossil_fstream_close(&stream);

    // Handle tail_lines printing
    if (tail_lines > 0 && count > 0 && cnotnull(lines)) {
        size_t start = count > (size_t)tail_lines ? count - tail_lines : 0;
        for (size_t i = start; i < count; i++) {
            if (number_lines) fossil_io_printf("{blue}%6zu{normal}\t%s\n", i + 1, lines[i]);
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
