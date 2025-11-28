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
// Maximized file type detector: supports more extensions, case-insensitive, and handles compound extensions
static ccstring detect_type(ccstring path) {
    size_t len = fossil_io_cstring_length(path);
    if (len < 2) return "unknown";

    // Lowercase copy for case-insensitive comparison
    cstring extbuf = fossil_io_cstring_create_safe("", 32);
    int extlen = 0;
    for (int i = (int)len - 1; i >= 0 && extlen < 31; i--) {
        if (path[i] == '.') {
            fossil_io_cstring_free(extbuf);
            extbuf = fossil_io_cstring_substring_safe(path, i, len - i, 32);
            cstring lower = fossil_io_cstring_to_lower_safe(extbuf, 32);
            fossil_io_cstring_free(extbuf);
            extbuf = lower;
            break;
        }
    }

    // Compound extensions (e.g., .tar.gz, .user.js)
    if (len >= 7 && fossil_io_cstring_iequals_safe(path + len - 7, ".tar.gz", 7)) {
        fossil_io_cstring_free(extbuf); return "archive";
    }
    if (len >= 8 && fossil_io_cstring_iequals_safe(path + len - 8, ".tar.bz2", 8)) {
        fossil_io_cstring_free(extbuf); return "archive";
    }
    if (len >= 9 && fossil_io_cstring_iequals_safe(path + len - 9, ".user.js", 9)) {
        fossil_io_cstring_free(extbuf); return "javascript-user-script";
    }
    if (len >= 8 && fossil_io_cstring_iequals_safe(path + len - 8, ".spec.js", 8)) {
        fossil_io_cstring_free(extbuf); return "javascript-test";
    }
    if (len >= 8 && fossil_io_cstring_iequals_safe(path + len - 8, ".test.js", 8)) {
        fossil_io_cstring_free(extbuf); return "javascript-test";
    }

    // Logs & Text
    if (fossil_io_cstring_equals_safe(extbuf, ".log", 32))  { fossil_io_cstring_free(extbuf); return "log"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".txt", 32))  { fossil_io_cstring_free(extbuf); return "text"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".md", 32))   { fossil_io_cstring_free(extbuf); return "markdown"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".rst", 32))  { fossil_io_cstring_free(extbuf); return "restructuredtext"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".csv", 32))  { fossil_io_cstring_free(extbuf); return "csv"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".tsv", 32))  { fossil_io_cstring_free(extbuf); return "tsv"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".json", 32)) { fossil_io_cstring_free(extbuf); return "json"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".xml", 32))  { fossil_io_cstring_free(extbuf); return "xml"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".yml", 32) || fossil_io_cstring_equals_safe(extbuf, ".yaml", 32)) { fossil_io_cstring_free(extbuf); return "yaml"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".toml", 32)) { fossil_io_cstring_free(extbuf); return "toml"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".ini", 32))  { fossil_io_cstring_free(extbuf); return "ini"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".cfg", 32))  { fossil_io_cstring_free(extbuf); return "config"; }

    // Code & Headers
    if (fossil_io_cstring_equals_safe(extbuf, ".c", 32))    { fossil_io_cstring_free(extbuf); return "c-code"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".h", 32))    { fossil_io_cstring_free(extbuf); return "c-header"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".cpp", 32))  { fossil_io_cstring_free(extbuf); return "cpp"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".cc", 32))   { fossil_io_cstring_free(extbuf); return "cpp"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".cxx", 32))  { fossil_io_cstring_free(extbuf); return "cpp"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".hpp", 32))  { fossil_io_cstring_free(extbuf); return "cpp-header"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".hh", 32))   { fossil_io_cstring_free(extbuf); return "cpp-header"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".hxx", 32))  { fossil_io_cstring_free(extbuf); return "cpp-header"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".py", 32))   { fossil_io_cstring_free(extbuf); return "python"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".pyw", 32))  { fossil_io_cstring_free(extbuf); return "python"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".ipynb", 32)){ fossil_io_cstring_free(extbuf); return "jupyter-notebook"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".js", 32))   { fossil_io_cstring_free(extbuf); return "javascript"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".ts", 32))   { fossil_io_cstring_free(extbuf); return "typescript"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".jsx", 32))  { fossil_io_cstring_free(extbuf); return "jsx"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".tsx", 32))  { fossil_io_cstring_free(extbuf); return "tsx"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".rb", 32))   { fossil_io_cstring_free(extbuf); return "ruby"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".java", 32)) { fossil_io_cstring_free(extbuf); return "java"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".cs", 32))   { fossil_io_cstring_free(extbuf); return "csharp"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".go", 32))   { fossil_io_cstring_free(extbuf); return "go"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".rs", 32))   { fossil_io_cstring_free(extbuf); return "rust"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".swift", 32)){ fossil_io_cstring_free(extbuf); return "swift"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".kt", 32))   { fossil_io_cstring_free(extbuf); return "kotlin"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".scala", 32)){ fossil_io_cstring_free(extbuf); return "scala"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".php", 32))  { fossil_io_cstring_free(extbuf); return "php"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".pl", 32))   { fossil_io_cstring_free(extbuf); return "perl"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".lua", 32))  { fossil_io_cstring_free(extbuf); return "lua"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".dart", 32)) { fossil_io_cstring_free(extbuf); return "dart"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".m", 32))    { fossil_io_cstring_free(extbuf); return "objective-c"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".mm", 32))   { fossil_io_cstring_free(extbuf); return "objective-c++"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".groovy", 32)){ fossil_io_cstring_free(extbuf); return "groovy"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".asm", 32))  { fossil_io_cstring_free(extbuf); return "assembly"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".s", 32))    { fossil_io_cstring_free(extbuf); return "assembly"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".f", 32) || fossil_io_cstring_equals_safe(extbuf, ".for", 32) || fossil_io_cstring_equals_safe(extbuf, ".f90", 32)) { fossil_io_cstring_free(extbuf); return "fortran"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".r", 32))    { fossil_io_cstring_free(extbuf); return "r"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".jl", 32))   { fossil_io_cstring_free(extbuf); return "julia"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".sh", 32))   { fossil_io_cstring_free(extbuf); return "shell-script"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".bash", 32)) { fossil_io_cstring_free(extbuf); return "shell-script"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".zsh", 32))  { fossil_io_cstring_free(extbuf); return "shell-script"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".bat", 32))  { fossil_io_cstring_free(extbuf); return "batch-script"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".ps1", 32) || fossil_io_cstring_equals_safe(extbuf, ".ps", 32)) { fossil_io_cstring_free(extbuf); return "powershell-script"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".make", 32) || fossil_io_cstring_equals_safe(extbuf, ".mk", 32)) { fossil_io_cstring_free(extbuf); return "makefile"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".dockerfile", 32)) { fossil_io_cstring_free(extbuf); return "dockerfile"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".gradle", 32)) { fossil_io_cstring_free(extbuf); return "gradle"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".cmake", 32)) { fossil_io_cstring_free(extbuf); return "cmake"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".vim", 32))   { fossil_io_cstring_free(extbuf); return "vimscript"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".emacs", 32) || fossil_io_cstring_equals_safe(extbuf, ".el", 32)) { fossil_io_cstring_free(extbuf); return "emacs-lisp"; }

    // Documents
    if (fossil_io_cstring_equals_safe(extbuf, ".pdf", 32))   { fossil_io_cstring_free(extbuf); return "pdf"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".doc", 32) || fossil_io_cstring_equals_safe(extbuf, ".docx", 32)) { fossil_io_cstring_free(extbuf); return "word"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".xls", 32) || fossil_io_cstring_equals_safe(extbuf, ".xlsx", 32)) { fossil_io_cstring_free(extbuf); return "excel"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".ppt", 32) || fossil_io_cstring_equals_safe(extbuf, ".pptx", 32)) { fossil_io_cstring_free(extbuf); return "powerpoint"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".odt", 32))   { fossil_io_cstring_free(extbuf); return "open-document"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".ods", 32))   { fossil_io_cstring_free(extbuf); return "open-spreadsheet"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".odp", 32))   { fossil_io_cstring_free(extbuf); return "open-presentation"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".epub", 32))  { fossil_io_cstring_free(extbuf); return "ebook"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".tex", 32))   { fossil_io_cstring_free(extbuf); return "latex"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".rtf", 32))   { fossil_io_cstring_free(extbuf); return "rtf"; }

    // Images
    if (fossil_io_cstring_equals_safe(extbuf, ".png", 32))   { fossil_io_cstring_free(extbuf); return "image"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".jpg", 32) || fossil_io_cstring_equals_safe(extbuf, ".jpeg", 32)) { fossil_io_cstring_free(extbuf); return "image"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".gif", 32))   { fossil_io_cstring_free(extbuf); return "image"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".bmp", 32))   { fossil_io_cstring_free(extbuf); return "image"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".svg", 32))   { fossil_io_cstring_free(extbuf); return "vector-image"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".webp", 32))  { fossil_io_cstring_free(extbuf); return "image"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".ico", 32))   { fossil_io_cstring_free(extbuf); return "icon"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".tiff", 32) || fossil_io_cstring_equals_safe(extbuf, ".tif", 32)) { fossil_io_cstring_free(extbuf); return "image"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".psd", 32))   { fossil_io_cstring_free(extbuf); return "photoshop"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".ai", 32))    { fossil_io_cstring_free(extbuf); return "illustrator"; }

    // Audio / Video
    if (fossil_io_cstring_equals_safe(extbuf, ".mp3", 32))   { fossil_io_cstring_free(extbuf); return "audio"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".wav", 32))   { fossil_io_cstring_free(extbuf); return "audio"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".ogg", 32))   { fossil_io_cstring_free(extbuf); return "audio"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".flac", 32))  { fossil_io_cstring_free(extbuf); return "audio"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".aac", 32))   { fossil_io_cstring_free(extbuf); return "audio"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".m4a", 32))   { fossil_io_cstring_free(extbuf); return "audio"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".wma", 32))   { fossil_io_cstring_free(extbuf); return "audio"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".mp4", 32))   { fossil_io_cstring_free(extbuf); return "video"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".mkv", 32))   { fossil_io_cstring_free(extbuf); return "video"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".avi", 32))   { fossil_io_cstring_free(extbuf); return "video"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".mov", 32))   { fossil_io_cstring_free(extbuf); return "video"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".wmv", 32))   { fossil_io_cstring_free(extbuf); return "video"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".webm", 32))  { fossil_io_cstring_free(extbuf); return "video"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".flv", 32))   { fossil_io_cstring_free(extbuf); return "video"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".3gp", 32))   { fossil_io_cstring_free(extbuf); return "video"; }

    // Compressed / Archives
    if (fossil_io_cstring_equals_safe(extbuf, ".zip", 32))   { fossil_io_cstring_free(extbuf); return "archive"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".tar", 32))   { fossil_io_cstring_free(extbuf); return "archive"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".gz", 32))    { fossil_io_cstring_free(extbuf); return "archive"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".bz2", 32))   { fossil_io_cstring_free(extbuf); return "archive"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".xz", 32))    { fossil_io_cstring_free(extbuf); return "archive"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".rar", 32))   { fossil_io_cstring_free(extbuf); return "archive"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".7z", 32))    { fossil_io_cstring_free(extbuf); return "archive"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".tgz", 32))   { fossil_io_cstring_free(extbuf); return "archive"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".tbz", 32))   { fossil_io_cstring_free(extbuf); return "archive"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".iso", 32))   { fossil_io_cstring_free(extbuf); return "disk-image"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".dmg", 32))   { fossil_io_cstring_free(extbuf); return "disk-image"; }

    // Misc
    if (fossil_io_cstring_equals_safe(extbuf, ".db", 32) || fossil_io_cstring_equals_safe(extbuf, ".sqlite", 32)) { fossil_io_cstring_free(extbuf); return "database"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".bak", 32))  { fossil_io_cstring_free(extbuf); return "backup"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".tmp", 32))  { fossil_io_cstring_free(extbuf); return "temporary"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".lock", 32)) { fossil_io_cstring_free(extbuf); return "lockfile"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".pid", 32))  { fossil_io_cstring_free(extbuf); return "pidfile"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".conf", 32)) { fossil_io_cstring_free(extbuf); return "config"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".env", 32))  { fossil_io_cstring_free(extbuf); return "envfile"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".crt", 32) || fossil_io_cstring_equals_safe(extbuf, ".pem", 32) || fossil_io_cstring_equals_safe(extbuf, ".key", 32)) { fossil_io_cstring_free(extbuf); return "certificate"; }
    if (fossil_io_cstring_equals_safe(extbuf, ".log", 32))  { fossil_io_cstring_free(extbuf); return "log"; }

    fossil_io_cstring_free(extbuf);
    // Default fallback
    return "unknown";
}

// ------------------------------------------------------------
// Utility: simple keyword frequency table
// ------------------------------------------------------------
typedef struct {
    char word[64];
    int count;
} keyword_t;

static int add_keyword(keyword_t *arr, int max, int *used, const char *word) {
    if (fossil_io_cstring_length(word) < 3) return 0;  // skip trivial words

    for (int i = 0; i < *used; i++) {
        if (fossil_io_cstring_iequals(arr[i].word, word)) {
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
// Utility: extract topics from text (simple frequency-based clustering)
// ------------------------------------------------------------
static ccstring *_extract_topics(const char *text, int *topic_count) {
    // Simple implementation: find most frequent keywords as "topics"
    keyword_t keywords[128];
    int kw_used = 0;

    char temp[8192];
    strncpy(temp, text, 8191);
    temp[8191] = 0;

    char *p = temp;
    while (*p) {
        if (!isalnum(*p)) *p = ' ';
        p++;
    }

    char *saveptr = NULL;
    char *tok = fossil_io_cstring_token(temp, " ", &saveptr);
    while (tok) {
        cstring lower_tok = fossil_io_cstring_to_lower(tok);
        add_keyword(keywords, 128, &kw_used, lower_tok);
        fossil_io_cstring_free(lower_tok);
        tok = fossil_io_cstring_token(NULL, " ", &saveptr);
    }

    int limit = kw_used < 5 ? kw_used : 5;
    ccstring *topics = fossil_sys_memory_alloc(sizeof(ccstring) * limit);
    for (int i = 0; i < limit; i++) {
        topics[i] = fossil_io_cstring_create(keywords[i].word);
    }
    *topic_count = limit;
    return topics;
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
    if (fossil_io_file_open(&fp, path, "readb")) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold,pos:top}Could not open file:{normal} %s\n", path);
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

    // Use detect_type helper for file type inference
    ccstring type = auto_detect ? detect_type(path) : "unknown";

    // For advanced analysis
    int ragebait = 0, clickbait = 0, spam = 0, woke = 0, bot = 0, sarcasm = 0, formal = 0;
    int snowflake = 0, offensive = 0, neutral = 0, hype = 0, quality = 0, political = 0;
    int conspiracy = 0, marketing = 0, technobabble = 0;
    int readability = 0, clarity = 0, quality_score = 0, passive_ratio = 0;
    char *summary = NULL, *key_sentence = NULL;
    const char *style = NULL, *tone = NULL, *readability_label = NULL;

    // Stats
    long word_count = 0;
    long max_line_length = 0;

    // ------------------------------
    // Read file (but do not output contents)
    // ------------------------------
    while (fossil_io_gets_from_stream(linebuf, 8192, &fp)) {
        line_count++;
        int len = (int)fossil_io_cstring_length(linebuf);
        char_count += len;
        if (do_stats && len > max_line_length) max_line_length = len;

        // Copy for entropy computation
        if (entused + len < 1024 * 1024) {
            memcpy(entbuf + entused, linebuf, len);
            entused += len;
        }

        // Keyword detection (simple tokenizing)
        if (extract_keywords || do_stats) {
            char temp[8192];
            strncpy(temp, linebuf, 8191);
            temp[8191] = 0;

            char *p = temp;
            while (*p) {
                if (!isalnum(*p)) *p = ' ';
                p++;
            }

            char *saveptr = NULL;
            char *tok = fossil_io_cstring_token(temp, " ", &saveptr);
            while (tok) {
                if (extract_keywords) {
                    cstring lower_tok = fossil_io_cstring_to_lower(tok);
                    add_keyword(keywords, 256, &kw_used, lower_tok);
                    fossil_io_cstring_free(lower_tok);
                }
                if (do_stats) word_count++;
                tok = fossil_io_cstring_token(NULL, " ", &saveptr);
            }
        }

        // Advanced analysis (per line)
        ragebait     += fossil_io_soap_detect_ragebait(linebuf);
        clickbait    += fossil_io_soap_detect_clickbait(linebuf);
        spam         += fossil_io_soap_detect_spam(linebuf);
        woke         += fossil_io_soap_detect_woke(linebuf);
        bot          += fossil_io_soap_detect_bot(linebuf);
        sarcasm      += fossil_io_soap_detect_sarcasm(linebuf);
        formal       += fossil_io_soap_detect_formal(linebuf);
        snowflake    += fossil_io_soap_detect_snowflake(linebuf);
        offensive    += fossil_io_soap_detect_offensive(linebuf);
        neutral      += fossil_io_soap_detect_neutral(linebuf);
        hype         += fossil_io_soap_detect_hype(linebuf);
        quality      += fossil_io_soap_detect_quality(linebuf);
        political    += fossil_io_soap_detect_political(linebuf);
        conspiracy   += fossil_io_soap_detect_conspiracy(linebuf);
        marketing    += fossil_io_soap_detect_marketing(linebuf);
        technobabble += fossil_io_soap_detect_technobabble(linebuf);

        if (limit_lines > 0 && line_count >= limit_lines)
            break;
    }

    fossil_io_file_close(&fp);

    // Whole-file analysis
    readability      = fossil_io_soap_readability_score((const char *)entbuf);
    readability_label= fossil_io_soap_readability_label((const char *)entbuf);
    clarity          = fossil_io_soap_clarity_score((const char *)entbuf);
    quality_score    = fossil_io_soap_quality_score((const char *)entbuf);
    passive_ratio    = fossil_io_soap_passive_voice_ratio((const char *)entbuf);
    style            = fossil_io_soap_analyze_style((const char *)entbuf);
    tone             = fossil_io_soap_detect_tone((const char *)entbuf);
    summary          = fossil_io_soap_summarize((const char *)entbuf);
    key_sentence     = fossil_io_soap_extract_key_sentence((const char *)entbuf);

    // Topic extraction using extract_topics utility
    ccstring *topics = NULL;
    int topic_count = 0;
    if (extract_topics) {
        topics = _extract_topics((const char *)entbuf, &topic_count);
    }

    // ------------------------------------------------------------
    // Output section (do not output file contents)
    // ------------------------------------------------------------
    // Generate AI-like summary response based on analysis values
    fossil_io_cstring_stream *ai_stream = fossil_io_cstring_stream_create(1024);
    fossil_io_cstring_stream_write_format(ai_stream,
        "This file \"%s\" is detected as type \"%s\". It contains %ld lines and %ld characters. "
        "The content has a readability score of %d (%s), clarity %d, and quality %d. "
        "Entropy is %.3f bits/byte, indicating %s randomness. "
        "The style is \"%s\" and the tone is \"%s\". "
        "Passive voice usage is %d%%. ",
        path, type, line_count, char_count,
        readability, readability_label ? readability_label : "unknown",
        clarity, quality_score,
        compute_entropy(entbuf, entused),
        compute_entropy(entbuf, entused) > 4.0 ? "high" : "low",
        style ? style : "unknown",
        tone ? tone : "unknown",
        passive_ratio
    );

    // Output stats if requested
    if (do_stats) {
        fossil_io_cstring_stream_write_format(ai_stream,
            "Word count: %ld. Max line length: %ld. ",
            word_count, max_line_length
        );
    }

    fossil_io_cstring_stream_write_format(ai_stream,
        "Flags detected: %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s. "
        "Top keywords: ",
        ragebait     ? "ragebait, "     : "",
        clickbait    ? "clickbait, "    : "",
        spam         ? "spam, "         : "",
        woke         ? "woke, "         : "",
        bot          ? "bot, "          : "",
        sarcasm      ? "sarcasm, "      : "",
        formal       ? "formal, "       : "",
        snowflake    ? "snowflake, "    : "",
        offensive    ? "offensive, "    : "",
        neutral      ? "neutral, "      : "",
        hype         ? "hype, "         : "",
        quality      ? "quality, "      : "",
        political    ? "political, "    : "",
        conspiracy   ? "conspiracy, "   : "",
        marketing    ? "marketing, "    : "",
        technobabble ? "technobabble, " : ""
    );

    // Append top keywords
    int kw_limit = kw_used < 5 ? kw_used : 5;
    for (int i = 0; i < kw_limit; i++) {
        fossil_io_cstring_stream_write(ai_stream, keywords[i].word);
        if (i + 1 < kw_limit)
            fossil_io_cstring_stream_write(ai_stream, ", ");
    }
    fossil_io_cstring_stream_write(ai_stream, ".");

    // Output topics if requested
    if (extract_topics && topics && topic_count > 0) {
        fossil_io_cstring_stream_write(ai_stream, " Topics: ");
        for (int i = 0; i < topic_count; i++) {
            fossil_io_cstring_stream_write(ai_stream, topics[i]);
            if (i + 1 < topic_count)
                fossil_io_cstring_stream_write(ai_stream, ", ");
        }
        fossil_io_cstring_stream_write(ai_stream, ".");
    }

    ccstring ai_response = fossil_io_cstring_stream_read(ai_stream);

    if (!output_fson) {
        fossil_io_clear_screen();
        fossil_io_move_cursor(1, 1);
        fossil_io_printf("{blue,bold,pos:top}AI Summary:{normal}\n%s\n", ai_response);
        fossil_io_flush();
    } else {
        fossil_io_printf("{yellow,bold}object:{normal} {\n");
        fossil_io_printf("  {cyan}ai_summary:{normal} cstr: \"{green}%s{normal}\"\n", ai_response);
        if (do_stats) {
            fossil_io_printf("  {cyan}stats:{normal} object: {\n");
            fossil_io_printf("    {magenta}lines:{normal} int: %ld\n", line_count);
            fossil_io_printf("    {magenta}chars:{normal} int: %ld\n", char_count);
            fossil_io_printf("    {magenta}words:{normal} int: %ld\n", word_count);
            fossil_io_printf("    {magenta}max_line_length:{normal} int: %ld\n", max_line_length);
            fossil_io_printf("  }\n");
        }
        if (extract_topics && topics && topic_count > 0) {
            fossil_io_printf("  {cyan}topics:{normal} array: [");
            for (int i = 0; i < topic_count; i++) {
                fossil_io_printf(" \"%s\"%s", topics[i], (i + 1 < topic_count) ? "," : "");
            }
            fossil_io_printf(" ]\n");
        }
        fossil_io_printf("{yellow,bold}}{normal}\n");
    }

    fossil_io_cstring_stream_free(ai_stream);
    fossil_sys_memory_free(linebuf);
    fossil_sys_memory_free(entbuf);
    if (summary) fossil_sys_memory_free(summary);
    if (key_sentence) fossil_sys_memory_free(key_sentence);
    return 0;
}

/**
 * @brief Generate structured summaries of text/code/log/document files.
 *
 * @param paths Array of file paths.
 * @param count Number of paths.
 * @param limit_lines Limit number of lines to analyze (0 = no limit).
 * @param auto_detect Enable automatic file-type inference.
 * @param extract_keywords Extract keyword list.
 * @param extract_topics Perform simple topic clustering.
 * @param stats Include file statistics.
 * @param output_fson Output structured FSON format.
 * @return int Status code.
 */
int fossil_shark_summary(ccstring *paths, int count,
                         int limit_lines,
                         bool auto_detect,
                         bool extract_keywords,
                         bool extract_topics,
                         bool stats,
                         bool output_fson) {
    int result = 0;
    for (int i = 0; i < count; i++) {
        int res = summarize_file(paths[i], limit_lines, auto_detect,
                                extract_keywords, extract_topics,
                                stats, output_fson);
        if (res != 0)
            result = res;
    }
    return result;
}
