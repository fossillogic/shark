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
#include <errno.h>

// Helper: read line from file, return dynamically allocated string
static cstring read_line(fossil_fstream_t *stream) {
    size_t size = 128;
    size_t len = 0;
    cstring line = (cstring)fossil_sys_memory_alloc(size);

    if (cunlikely(!cnotnull(line))) {
        return cnull;
    }

    int c;
    while ((c = fossil_io_getc(stream)) != EOF) {
        if (len + 1 >= size) {
            size *= 2;
            cstring new_line = (cstring)fossil_sys_memory_realloc(line, size);
            if (cunlikely(!cnotnull(new_line))) {
                fossil_sys_memory_free(line);
                return cnull;
            }
            line = new_line;
        }
        line[len++] = (char)c;
        if (c == '\n') break;
    }
    
    if (len == 0 && c == EOF) { 
        fossil_sys_memory_free(line); 
        return cnull; 
    }
    line[len] = cterm;
    
    // Trim whitespace from the line
    fossil_io_trim(line);
    
    return line;
}

// Helper: compare lines with optional case-insensitivity
static bool line_equal(ccstring a, ccstring b, bool ignore_case) {
    if (!cnotnull(a) || !cnotnull(b)) return false;
    
    if (!ignore_case) return fossil_io_cstring_compare(a, b) == 0;
    return fossil_io_cstring_case_compare(a, b) == 0;
}

/**
 * Compare two files (text or binary)
 */
int fossil_shark_compare(ccstring path1, ccstring path2,
                         bool text_diff, bool binary_diff,
                         int context_lines, bool ignore_case) {
    if (!cnotnull(path1) || !cnotnull(path2)) {
        fossil_io_printf("{red}Error: Two paths must be specified.{normal}\n");
        return 1;
    }

    struct stat st1, st2;
    if (cunlikely(stat(path1, &st1) != 0 || stat(path2, &st2) != 0)) {
        fossil_io_printf("{red}Error: Failed to get file information: %s{normal}\n", strerror(errno));
        return errno;
    }

    if (cunlikely(!S_ISREG(st1.st_mode) || !S_ISREG(st2.st_mode))) {
        fossil_io_printf("{red}Error: Currently only regular files are supported.{normal}\n");
        return 1;
    }

    if (binary_diff) {
        fossil_fstream_t f1, f2;
        if (cunlikely(fossil_fstream_open(&f1, path1, "rb") != 0 || 
                      fossil_fstream_open(&f2, path2, "rb") != 0)) { 
            fossil_io_printf("{red}Error: Failed to open files: %s{normal}\n", strerror(errno));
            return errno; 
        }

        int diff_found = 0;
        int c1, c2;
        size_t pos = 0;
        while ((c1 = fossil_io_getc(&f1)) != EOF && (c2 = fossil_io_getc(&f2)) != EOF) {
            if (c1 != c2) {
                fossil_io_printf("{cyan}Binary difference at byte %zu: %02x != %02x{normal}\n", pos, c1 & 0xFF, c2 & 0xFF);
                diff_found = 1;
                break;
            }
            pos++;
        }
        if (!diff_found && (fossil_io_getc(&f1) != EOF || fossil_io_getc(&f2) != EOF)) diff_found = 1;
        fossil_fstream_close(&f1);
        fossil_fstream_close(&f2);
        return diff_found;
    }

    if (text_diff) {
        fossil_fstream_t f1, f2;
        if (cunlikely(fossil_fstream_open(&f1, path1, "r") != 0 || 
                      fossil_fstream_open(&f2, path2, "r") != 0)) { 
            fossil_io_printf("{red}Error: Failed to open files: %s{normal}\n", strerror(errno));
            return errno; 
        }

        cstring line1, line2;
        size_t line_no = 1;
        int differences = 0;

        while (1) {
            line1 = read_line(&f1);
            line2 = read_line(&f2);

            if (!cnotnull(line1) && !cnotnull(line2)) break;

            if (!cnotnull(line1) || !cnotnull(line2) || !line_equal(line1, line2, ignore_case)) {
                fossil_io_printf("{blue}Difference at line %zu:{normal}\n", line_no);
                if (cnotnull(line1)) fossil_io_printf("{cyan}< %s{normal}\n", line1);
                if (cnotnull(line2)) fossil_io_printf("{cyan}> %s{normal}\n", line2);
                differences++;
            }

            if (cnotnull(line1)) {
                fossil_sys_memory_free(line1);
                cdrop(line1);
            }
            if (cnotnull(line2)) {
                fossil_sys_memory_free(line2);
                cdrop(line2);
            }
            line_no++;
        }

        fossil_fstream_close(&f1);
        fossil_fstream_close(&f2);
        return differences > 0 ? 1 : 0;
    }

    fossil_io_printf("{red}Error: Specify at least text_diff or binary_diff.{normal}\n");
    return 1;
}
