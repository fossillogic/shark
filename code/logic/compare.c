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

// Helper: check if file is regular file (cross-platform)
static bool is_regular_file(ccstring path) {
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    return (attrs != INVALID_FILE_ATTRIBUTES) && 
           !(attrs & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat st;
    if (stat(path, &st) != 0) return false;
    return S_ISREG(st.st_mode);
#endif
}

// Helper: get file size (cross-platform)
static size_t get_file_size(ccstring path) {
#ifdef _WIN32
    HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, 
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return 0;
    
    LARGE_INTEGER size;
    if (!GetFileSizeEx(hFile, &size)) {
        CloseHandle(hFile);
        return 0;
    }
    CloseHandle(hFile);
    return (size_t)size.QuadPart;
#else
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return (size_t)st.st_size;
#endif
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

    if (cunlikely(!is_regular_file(path1) || !is_regular_file(path2))) {
        fossil_io_printf("{red}Error: Failed to access files or not regular files.{normal}\n");
        return 1;
    }

    if (binary_diff) {
        fossil_fstream_t f1, f2;
        if (cunlikely(fossil_fstream_open(&f1, path1, "rb") != 0 || 
                      fossil_fstream_open(&f2, path2, "rb") != 0)) { 
            fossil_io_printf("{red}Error: Failed to open files for binary comparison.{normal}\n");
            return 1; 
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
            fossil_io_printf("{red}Error: Failed to open files for text comparison.{normal}\n");
            return 1; 
        }

        // Read all lines first
        cstring *lines1 = cnull, *lines2 = cnull;
        size_t count1 = 0, count2 = 0;
        size_t capacity1 = 64, capacity2 = 64;
        
        lines1 = (cstring*)fossil_sys_memory_alloc(capacity1 * sizeof(cstring));
        lines2 = (cstring*)fossil_sys_memory_alloc(capacity2 * sizeof(cstring));
        
        if (!cnotnull(lines1) || !cnotnull(lines2)) {
            if (cnotnull(lines1)) fossil_sys_memory_free(lines1);
            if (cnotnull(lines2)) fossil_sys_memory_free(lines2);
            fossil_fstream_close(&f1);
            fossil_fstream_close(&f2);
            return 1;
        }

        cstring line;
        while ((line = read_line(&f1)) != cnull) {
            if (count1 >= capacity1) {
                capacity1 *= 2;
                cstring *new_lines = (cstring*)fossil_sys_memory_realloc(lines1, capacity1 * sizeof(cstring));
                if (!cnotnull(new_lines)) {
                    // Cleanup on realloc failure
                    for (size_t i = 0; i < count1; i++) fossil_sys_memory_free(lines1[i]);
                    fossil_sys_memory_free(lines1);
                    fossil_sys_memory_free(lines2);
                    fossil_fstream_close(&f1);
                    fossil_fstream_close(&f2);
                    return 1;
                }
                lines1 = new_lines;
            }
            lines1[count1++] = line;
        }

        while ((line = read_line(&f2)) != cnull) {
            if (count2 >= capacity2) {
                capacity2 *= 2;
                cstring *new_lines = (cstring*)fossil_sys_memory_realloc(lines2, capacity2 * sizeof(cstring));
                if (!cnotnull(new_lines)) {
                    // Cleanup on realloc failure
                    for (size_t i = 0; i < count1; i++) fossil_sys_memory_free(lines1[i]);
                    for (size_t i = 0; i < count2; i++) fossil_sys_memory_free(lines2[i]);
                    fossil_sys_memory_free(lines1);
                    fossil_sys_memory_free(lines2);
                    fossil_fstream_close(&f1);
                    fossil_fstream_close(&f2);
                    return 1;
                }
                lines2 = new_lines;
            }
            lines2[count2++] = line;
        }

        fossil_fstream_close(&f1);
        fossil_fstream_close(&f2);

        int differences = 0;
        size_t max_lines = count1 > count2 ? count1 : count2;
        
        for (size_t i = 0; i < max_lines; i++) {
            cstring line1 = i < count1 ? lines1[i] : cnull;
            cstring line2 = i < count2 ? lines2[i] : cnull;
            
            if (!cnotnull(line1) || !cnotnull(line2) || !line_equal(line1, line2, ignore_case)) {
                // Print context lines before difference
                int start_context = (int)i - context_lines;
                if (start_context < 0) start_context = 0;
                
                for (int ctx = start_context; ctx < (int)i; ctx++) {
                    if (ctx < (int)count1 && cnotnull(lines1[ctx])) {
                        fossil_io_printf("  %d: %s\n", ctx + 1, lines1[ctx]);
                    }
                }
                
                fossil_io_printf("{blue}Difference at line %zu:{normal}\n", i + 1);
                if (cnotnull(line1)) fossil_io_printf("{cyan}< %s{normal}\n", line1);
                if (cnotnull(line2)) fossil_io_printf("{cyan}> %s{normal}\n", line2);
                
                // Print context lines after difference
                int end_context = (int)i + context_lines + 1;
                if (end_context > (int)max_lines) end_context = (int)max_lines;
                
                for (int ctx = (int)i + 1; ctx < end_context; ctx++) {
                    if (ctx < (int)count1 && cnotnull(lines1[ctx])) {
                        fossil_io_printf("  %d: %s\n", ctx + 1, lines1[ctx]);
                    }
                }
                fossil_io_printf("\n");
                differences++;
            }
        }

        // Clean up
        for (size_t i = 0; i < count1; i++) {
            if (cnotnull(lines1[i])) fossil_sys_memory_free(lines1[i]);
        }
        for (size_t i = 0; i < count2; i++) {
            if (cnotnull(lines2[i])) fossil_sys_memory_free(lines2[i]);
        }
        fossil_sys_memory_free(lines1);
        fossil_sys_memory_free(lines2);
        
        return differences > 0 ? 1 : 0;
    }

    fossil_io_printf("{red}Error: Specify at least text_diff or binary_diff.{normal}\n");
    return 1;
}
