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
#include <string.h>
#include <stdbool.h>

/* ------------------------------------------------------------
 * Fossil Shark Pipe Command
 * ------------------------------------------------------------ */
int fossil_shark_pipe(
    const char* input_file,
    const char* output_file,
    const char* filter,
    bool tee,
    bool output_json,
    bool append
) {
    FILE *in = stdin;
    FILE *out = stdout;
    FILE *tmp_out = NULL;
    char buffer[4096];
    size_t n;

    /* Open input file if specified */
    if (input_file) {
        in = fopen(input_file, "rb");
        if (!in) {
            perror("Failed to open input file");
            return -1;
        }
    }

    /* Open output file if specified */
    if (output_file) {
        out = fopen(output_file, append ? "ab" : "wb");
        if (!out) {
            perror("Failed to open output file");
            if (input_file) fclose(in);
            return -2;
        }
    }

    /* Optional temporary stream for tee */
    if (tee && output_file) {
        tmp_out = stdout;
    }

    /* Process input line by line */
    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        /* Apply filter if provided (simple strstr check per block) */
        if (filter) {
            char filtered[4096];
            size_t filtered_len = 0;
            for (size_t i = 0; i < n; i++) {
                filtered[i] = buffer[i];
            }
            filtered_len = n;

            /* Simple inline filter: skip block if filter not found */
            if (!memmem(filtered, filtered_len, filter, strlen(filter))) {
                continue;
            }

            fwrite(filtered, 1, filtered_len, out);
            if (tmp_out) fwrite(filtered, 1, filtered_len, tmp_out);
        } else {
            /* No filter: write directly */
            fwrite(buffer, 1, n, out);
            if (tmp_out) fwrite(buffer, 1, n, tmp_out);
        }
    }

    /* JSON output wrapping */
    if (output_json) {
        fprintf(out, "\n{\"status\":\"ok\"}\n");
        if (tmp_out) fprintf(tmp_out, "\n{\"status\":\"ok\"}\n");
    }

    /* Cleanup */
    if (input_file) fclose(in);
    if (output_file) fclose(out);

    return 0;
}

/* ------------------------------------------------------------
 * Simple memmem implementation for portability
 * ------------------------------------------------------------ */
#ifndef _GNU_SOURCE
void* memmem(const void* haystack, size_t haystacklen, const void* needle, size_t needlelen) {
    if (needlelen == 0) return (void*)haystack;
    for (size_t i = 0; i + needlelen <= haystacklen; i++) {
        if (memcmp((char*)haystack + i, needle, needlelen) == 0) return (char*)haystack + i;
    }
    return NULL;
}
#endif
