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
static const char* get_mime_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    ext++;
    if (strcasecmp(ext, "txt") == 0) return "text/plain";
    if (strcasecmp(ext, "c") == 0 || strcasecmp(ext, "h") == 0) return "text/plain";
    if (strcasecmp(ext, "html") == 0) return "text/html";
    if (strcasecmp(ext, "json") == 0) return "application/json";
    if (strcasecmp(ext, "jpg") == 0 || strcasecmp(ext, "jpeg") == 0) return "image/jpeg";
    if (strcasecmp(ext, "png") == 0) return "image/png";
    if (strcasecmp(ext, "zip") == 0) return "application/zip";
    return "application/octet-stream";
}

/**
 * Examine file contents, type, or metadata
 * @param path Path to the file to introspect
 * @param show_head_lines Number of lines from the start to display (0 for none)
 * @param show_tail_lines Number of lines from the end to display (0 for none)
 * @param count_lines_words_bytes Count lines, words, and bytes if true
 * @param show_file_type Determine file type (e.g., MIME or description)
 * @param output_fson Output metadata in FSON format
 * @return 0 on success, non-zero on error
 */
int fossil_shark_introspect(const char *path, int show_head_lines,
                             int show_tail_lines, bool count_lines_words_bytes,
                             bool show_file_type, bool output_fson)
{
    if (!path) return -1;

    struct stat st;
    if (stat(path, &st) != 0) return errno;

    FILE *fp = fopen(path, "r");
    if (!fp && (show_head_lines > 0 || show_tail_lines > 0 || count_lines_words_bytes)) return errno;

    unsigned long lines = 0, words = 0, bytes = 0;
    char buffer[4096];
    char **tail_buffer = NULL;
    int tail_index = 0;

    if (show_tail_lines > 0) {
        tail_buffer = malloc(sizeof(char*) * show_tail_lines);
        if (!tail_buffer) {
            if (fp) fclose(fp);
            return -1;
        }
        for (int i = 0; i < show_tail_lines; i++) tail_buffer[i] = NULL;
    }

    int head_count = 0;
    while (fp && fgets(buffer, sizeof(buffer), fp)) {
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
            printf("%s", buffer);
            head_count++;
        }

        if (show_tail_lines > 0) {
            // store last N lines in circular buffer
            free(tail_buffer[tail_index]);
            tail_buffer[tail_index] = fossil_io_cstring_dup(buffer);
            tail_index = (tail_index + 1) % show_tail_lines;
        }
    }

    if (show_tail_lines > 0) {
        printf("\n-- Tail last %d lines --\n", show_tail_lines);
        int start = tail_index;
        for (int i = 0; i < show_tail_lines; i++) {
            int idx = (start + i) % show_tail_lines;
            if (tail_buffer[idx]) printf("%s", tail_buffer[idx]);
        }
    }

    if (fp) fclose(fp);

    // Output metadata
    if (output_fson) {
        printf("{\n");
        printf("  \"path\": \"%s\",\n", path);
        printf("  \"size\": %lld,\n", (long long)st.st_size);
        printf("  \"lines\": %lu,\n", lines);
        printf("  \"words\": %lu,\n", words);
        printf("  \"bytes\": %lu", bytes);
        if (show_file_type) {
            printf(",\n  \"type\": \"%s\"", get_mime_type(path));
        }
        printf("\n}\n");
    } else {
        printf("File: %s\n", path);
        printf("Size: %lld bytes\n", (long long)st.st_size);
        if (count_lines_words_bytes) {
            printf("Lines: %lu Words: %lu Bytes: %lu\n", lines, words, bytes);
        }
        if (show_file_type) {
            printf("Type: %s\n", get_mime_type(path));
        }
    }

    if (tail_buffer) {
        for (int i = 0; i < show_tail_lines; i++) free(tail_buffer[i]);
        free(tail_buffer);
    }

    return 0;
}
