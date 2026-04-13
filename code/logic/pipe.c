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
#include "fossil/code/pipe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef HAVE_MEMMEM
static void* fossil_memmem(const void* haystack, size_t haystacklen,
                           const void* needle, size_t needlelen)
{
    if (needlelen == 0) return (void*)haystack;

    for (size_t i = 0; i + needlelen <= haystacklen; i++)
    {
        if (memcmp((const char*)haystack + i, needle, needlelen) == 0)
            return (void*)((const char*)haystack + i);
    }
    return NULL;
}
#endif

/* ------------------------------------------------------------
 * Fossil Spino Pipe Command (Media-Aware)
 * ------------------------------------------------------------ */
int fossil_spino_pipe(
    const char* input_file,
    const char* output_file,
    const char* filter,
    bool tee,
    const char* media,   /* "text", "json", "fson" */
    bool append
) {
    FILE *in = stdin;
    FILE *out = stdout;
    FILE *tee_out = NULL;

    char buffer[4096];
    size_t n;

    const char* fmt = (media) ? media : "text";

    const char* needle = filter;
    size_t needle_len = (needle) ? strlen(needle) : 0;

    /* Open input */
    if (input_file) {
        in = fopen(input_file, "rb");
        if (!in) {
            perror("Failed to open input file");
            return -1;
        }
    }

    /* Open output */
    if (output_file) {
        out = fopen(output_file, append ? "ab" : "wb");
        if (!out) {
            perror("Failed to open output file");
            if (input_file) fclose(in);
            return -2;
        }
    }

    /* Tee setup */
    if (tee && output_file) {
        tee_out = stdout;
    }

    /* Overlap buffer for boundary-safe matching */
    char overlap[256];
    size_t overlap_len = 0;

    /* Stats (useful for structured output) */
    size_t total_bytes = 0;
    size_t total_chunks = 0;
    size_t passed_chunks = 0;

    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {

        total_bytes += n;
        total_chunks++;

        /* Combine overlap + current chunk */
        char combined[4096 + 256];
        size_t combined_len = 0;

        if (overlap_len > 0) {
            memcpy(combined, overlap, overlap_len);
            combined_len += overlap_len;
        }

        memcpy(combined + combined_len, buffer, n);
        combined_len += n;

        bool pass = true;

        if (needle) {
            if (!fossil_memmem(combined, combined_len, needle, needle_len)) {
                pass = false;
            }
        }

        if (pass) {
            fwrite(buffer, 1, n, out);
            if (tee_out) fwrite(buffer, 1, n, tee_out);
            passed_chunks++;
        }

        /* Maintain overlap */
        if (needle_len > 1) {
            overlap_len = (needle_len - 1 < combined_len)
                          ? (needle_len - 1)
                          : combined_len;

            memcpy(
                overlap,
                combined + combined_len - overlap_len,
                overlap_len
            );
        }
    }

    /* Emit structured footer (instead of raw JSON toggle) */
    if (strcmp(fmt, "json") == 0) {
        fprintf(out,
            "\n{\"status\":\"ok\",\"bytes\":%zu,\"chunks\":%zu,\"passed\":%zu}\n",
            total_bytes, total_chunks, passed_chunks
        );
        if (tee_out) {
            fprintf(tee_out,
                "\n{\"status\":\"ok\",\"bytes\":%zu,\"chunks\":%zu,\"passed\":%zu}\n",
                total_bytes, total_chunks, passed_chunks
            );
        }

    } else if (strcmp(fmt, "fson") == 0) {
        fprintf(out,
            "\nstatus:cstr=ok bytes:u32=%zu chunks:u32=%zu passed:u32=%zu\n",
            total_bytes, total_chunks, passed_chunks
        );
        if (tee_out) {
            fprintf(tee_out,
                "\nstatus:cstr=ok bytes:u32=%zu chunks:u32=%zu passed:u32=%zu\n",
                total_bytes, total_chunks, passed_chunks
            );
        }

    } else {
        /* text (default) — minimal, non-intrusive */
        fprintf(out, "\n[shark:pipe] OK (%zu bytes processed)\n", total_bytes);
        if (tee_out) {
            fprintf(tee_out, "\n[shark:pipe] OK (%zu bytes processed)\n", total_bytes);
        }
    }

    if (input_file) fclose(in);
    if (output_file) fclose(out);

    return 0;
}
