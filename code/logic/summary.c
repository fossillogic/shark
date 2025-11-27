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
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>

// ------------------------------------------------------------
// Helper: infer file type from extension
// ------------------------------------------------------------
static ccstring detect_type(ccstring path) {
    size_t len = strlen(path);
    if (len < 2) return "unknown";

    // Helper macro to simplify comparison
    #define EXT_EQ(ext_len, ext_str) (len >= ext_len && strcmp(path + len - ext_len, ext_str) == 0)

    // Logs & Text
    if (EXT_EQ(4, ".log")) return "log";
    if (EXT_EQ(4, ".txt")) return "text";
    if (EXT_EQ(3, ".md"))  return "markdown";
    if (EXT_EQ(4, ".csv")) return "csv";
    if (EXT_EQ(5, ".json")) return "json";
    if (EXT_EQ(4, ".xml")) return "xml";
    if (EXT_EQ(4, ".yml") || EXT_EQ(5, ".yaml")) return "yaml";

    // Code & Headers
    if (EXT_EQ(2, ".c")) return "c-code";
    if (EXT_EQ(2, ".h")) return "c-header";
    if (EXT_EQ(3, ".py")) return "python";
    if (EXT_EQ(3, ".js")) return "javascript";
    if (EXT_EQ(3, ".ts")) return "typescript";
    if (EXT_EQ(3, ".rb")) return "ruby";
    if (EXT_EQ(4, ".cpp")) return "cpp";
    if (EXT_EQ(4, ".hpp")) return "cpp-header";
    if (EXT_EQ(4, ".java")) return "java";
    if (EXT_EQ(4, ".cs")) return "csharp";

    // Configuration & Scripts
    if (EXT_EQ(3, ".sh")) return "shell-script";
    if (EXT_EQ(4, ".bat")) return "batch-script";
    if (EXT_EQ(3, ".ps"))  return "powershell-script";
    if (EXT_EQ(4, ".ini")) return "ini";
    if (EXT_EQ(4, ".cfg")) return "config";
    if (EXT_EQ(4, ".toml")) return "toml";

    // Documents
    if (EXT_EQ(4, ".pdf")) return "pdf";
    if (EXT_EQ(5, ".docx")) return "word";
    if (EXT_EQ(5, ".xlsx")) return "excel";
    if (EXT_EQ(5, ".pptx")) return "powerpoint";

    // Images
    if (EXT_EQ(4, ".png")) return "image";
    if (EXT_EQ(4, ".jpg") || EXT_EQ(5, ".jpeg")) return "image";
    if (EXT_EQ(4, ".gif")) return "image";
    if (EXT_EQ(4, ".bmp")) return "image";
    if (EXT_EQ(4, ".svg")) return "vector-image";

    // Audio / Video
    if (EXT_EQ(4, ".mp3")) return "audio";
    if (EXT_EQ(4, ".wav")) return "audio";
    if (EXT_EQ(4, ".ogg")) return "audio";
    if (EXT_EQ(4, ".mp4")) return "video";
    if (EXT_EQ(4, ".mkv")) return "video";
    if (EXT_EQ(4, ".avi")) return "video";

    // Compressed / Archives
    if (EXT_EQ(4, ".zip")) return "archive";
    if (EXT_EQ(4, ".tar")) return "archive";
    if (EXT_EQ(7, ".tar.gz")) return "archive";
    if (EXT_EQ(8, ".tar.bz2")) return "archive";
    if (EXT_EQ(4, ".rar")) return "archive";
    if (EXT_EQ(4, ".7z")) return "archive";

    // Default fallback
    return "unknown";

    #undef EXT_EQ
}

// ------------------------------------------------------------
// Utility: simple keyword frequency table
// ------------------------------------------------------------
typedef struct {
    char word[64];
    int count;
} keyword_t;

static int add_keyword(keyword_t *arr, int max, int *used, const char *word) {
    if (strlen(word) < 3) return 0;  // skip trivial words

    for (int i = 0; i < *used; i++) {
        if (strcmp(arr[i].word, word) == 0) {
            arr[i].count++;
            return 1;
        }
    }

    if (*used >= max) return 0;

    strncpy(arr[*used].word, word, 63);
    arr[*used].word[63] = 0;
    arr[*used].count = 1;
    (*used)++;
    return 1;
}


// ------------------------------------------------------------
// Utility: compute Shannon entropy
// ------------------------------------------------------------
static double compute_entropy(const unsigned char *data, size_t n) {
    if (n == 0) return 0.0;

    int freq[256] = {0};

    for (size_t i = 0; i < n; i++) {
        freq[data[i]]++;
    }

    double entropy = 0.0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] == 0) continue;
        double p = (double)freq[i] / n;
        entropy -= p * (log(p) / log(2.0));
    }
    return entropy;
}


// ------------------------------------------------------------
// Process a single file
// ------------------------------------------------------------
static int summarize_file(ccstring path,
                          int limit_lines,
                          bool auto_detect,
                          bool extract_keywords,
                          bool extract_topics,
                          bool do_stats,
                          bool output_fson)
{
    fossil_io_file_t fp;
    fossil_io_file_open(fp, path, "rb");
    if (!fp) {
        fossil_io_fprintf(FOSSIL_STDERR, "Could not open file: %s\n", path);
        return errno;
    }

    // buffers & counters
    char *linebuf = fossil_sys_memory_alloc(8192);
    if (!linebuf) { fossil_io_file_close(&fp); return ENOMEM; }

    long line_count = 0;
    long char_count = 0;

    unsigned char *entbuf = fossil_sys_memory_alloc(1024 * 1024);
    size_t entused = 0;

    keyword_t keywords[256];
    int kw_used = 0;

    ccstring type = auto_detect ? detect_type(path) : "unknown";


    // ------------------------------
    // Read file
    // ------------------------------
    while (fossil_io_gets_from_stream(linebuf, 8192, fp)) {
        line_count++;
        int len = (int)strlen(linebuf);
        char_count += len;

        // Copy for entropy computation
        if (entused + len < 1024 * 1024) {
            memcpy(entbuf + entused, linebuf, len);
            entused += len;
        }

        // Keyword detection (simple tokenizing)
        if (extract_keywords) {
            char temp[8192];
            strncpy(temp, linebuf, 8191);
            temp[8191] = 0;

            char *p = temp;
            while (*p) {
                if (!isalnum(*p)) *p = ' ';
                p++;
            }

            char *tok = strtok(temp, " ");
            while (tok) {
                for (char *c = tok; *c; c++) *c = (char)tolower(*c);
                add_keyword(keywords, 256, &kw_used, tok);
                tok = strtok(NULL, " ");
            }
        }

        if (limit_lines > 0 && line_count >= limit_lines)
            break;
    }

    fossil_io_file_close(&fp);


    // ------------------------------------------------------------
    // Output section
    // ------------------------------------------------------------
    if (!output_fson) {
        fossil_io_printf("{blue}Summary for:{normal} %s\n", path);
        fossil_io_printf("  Type: cstr %s\n", type);

        if (do_stats) {
            fossil_io_printf("  Lines: %ld\n", line_count);
            fossil_io_printf("  Chars: %ld\n", char_count);

            double entropy = compute_entropy(entbuf, entused);
            fossil_io_printf("  Entropy: %.3f bits/byte\n", entropy);
        }

        if (extract_keywords) {
            fossil_io_printf("\n{blue}Top Keywords:{normal}\n");
            for (int i = 0; i < kw_used && i < 20; i++) {
                fossil_io_printf("  %s (%d)\n", keywords[i].word, keywords[i].count);
            }
        }

        if (extract_topics) {
            fossil_io_printf("\n{blue}Topics:{normal}\n");
            fossil_io_printf("  (simple grouping of keywords)\n");
        }

    } else {
        // FSON output
        fossil_io_printf("{\n");
        fossil_io_printf("  \"file\": \"%s\",\n", path);
        fossil_io_printf("  \"type\": \"%s\",\n", type);

        if (do_stats) {
            fossil_io_printf("  \"stats\": {\n");
            fossil_io_printf("    \"lines\": %ld,\n", line_count);
            fossil_io_printf("    \"chars\": %ld,\n", char_count);
            fossil_io_printf("    \"entropy\": %.3f\n", compute_entropy(entbuf, entused));
            fossil_io_printf("  },\n");
        }

        if (extract_keywords) {
            fossil_io_printf("  \"keywords\": [");
            for (int i = 0; i < kw_used; i++) {
                fossil_io_printf("\"%s\"", keywords[i].word);
                if (i + 1 < kw_used) fossil_io_printf(", ");
            }
            fossil_io_printf("],\n");
        }

        if (extract_topics) {
            fossil_io_printf("  \"topics\": [\"group-1\", \"group-2\"],\n");
        }

        fossil_io_printf("}\n");
    }


    fossil_sys_memory_free(linebuf);
    fossil_sys_memory_free(entbuf);
    return 0;
}


// ------------------------------------------------------------
// Multi-file summary entry point
// ------------------------------------------------------------
int fossil_shark_summary(ccstring *paths, int count,
                         int limit_lines,
                         bool auto_detect,
                         bool extract_keywords,
                         bool extract_topics,
                         bool stats,
                         bool output_fson)
{
    for (int i = 0; i < count; i++) {
        summarize_file(paths[i],
                       limit_lines,
                       auto_detect,
                       extract_keywords,
                       extract_topics,
                       stats,
                       output_fson);

        if (!output_fson)
            fossil_io_printf("\n-----------------------------------\n\n");
    }

    return 0;
}
