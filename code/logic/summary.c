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
    if (fossil_io_file_open(&fp, path, "readb")) {
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

    // For advanced analysis
    int ragebait = 0, clickbait = 0, spam = 0, woke = 0, bot = 0, sarcasm = 0, formal = 0;
    int snowflake = 0, offensive = 0, neutral = 0, hype = 0, quality = 0, political = 0;
    int conspiracy = 0, marketing = 0, technobabble = 0;
    int readability = 0, clarity = 0, quality_score = 0, passive_ratio = 0;
    char *summary = NULL, *key_sentence = NULL;
    const char *style = NULL, *tone = NULL, *readability_label = NULL;

    // ------------------------------
    // Read file
    // ------------------------------
    while (fossil_io_gets_from_stream(linebuf, 8192, &fp)) {
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

            fossil_io_printf("  Readability: %d (%s)\n", readability, readability_label);
            fossil_io_printf("  Clarity: %d\n", clarity);
            fossil_io_printf("  Quality: %d\n", quality_score);
            fossil_io_printf("  Passive voice: %d%%\n", passive_ratio);
            fossil_io_printf("  Style: %s\n", style ? style : "unknown");
            fossil_io_printf("  Tone: %s\n", tone ? tone : "unknown");
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

        fossil_io_printf("\n{blue}Summary:{normal}\n  %s\n", summary ? summary : "(none)");
        fossil_io_printf("\n{blue}Key Sentence:{normal}\n  %s\n", key_sentence ? key_sentence : "(none)");

        fossil_io_printf("\n{blue}Content Flags:{normal}\n");
        fossil_io_printf("  Ragebait: %s\n", ragebait ? "yes" : "no");
        fossil_io_printf("  Clickbait: %s\n", clickbait ? "yes" : "no");
        fossil_io_printf("  Spam: %s\n", spam ? "yes" : "no");
        fossil_io_printf("  Woke: %s\n", woke ? "yes" : "no");
        fossil_io_printf("  Bot: %s\n", bot ? "yes" : "no");
        fossil_io_printf("  Sarcasm: %s\n", sarcasm ? "yes" : "no");
        fossil_io_printf("  Formal: %s\n", formal ? "yes" : "no");
        fossil_io_printf("  Snowflake: %s\n", snowflake ? "yes" : "no");
        fossil_io_printf("  Offensive: %s\n", offensive ? "yes" : "no");
        fossil_io_printf("  Neutral: %s\n", neutral ? "yes" : "no");
        fossil_io_printf("  Hype: %s\n", hype ? "yes" : "no");
        fossil_io_printf("  Quality: %s\n", quality ? "yes" : "no");
        fossil_io_printf("  Political: %s\n", political ? "yes" : "no");
        fossil_io_printf("  Conspiracy: %s\n", conspiracy ? "yes" : "no");
        fossil_io_printf("  Marketing: %s\n", marketing ? "yes" : "no");
        fossil_io_printf("  Technobabble: %s\n", technobabble ? "yes" : "no");

    } else {
        // FSON output
        fossil_io_printf("object: {\n");
        fossil_io_printf("  file: cstr: \"%s\",\n", path);
        fossil_io_printf("  type: cstr: \"%s\",\n", type);

        if (do_stats) {
            fossil_io_printf("  stats: object: {\n");
            fossil_io_printf("    lines: i64: %ld,\n", line_count);
            fossil_io_printf("    chars: i64: %ld,\n", char_count);
            fossil_io_printf("    entropy: f64: %.3f,\n", compute_entropy(entbuf, entused));
            fossil_io_printf("    readability: i32: %d,\n", readability);
            fossil_io_printf("    clarity: i32: %d,\n", clarity);
            fossil_io_printf("    quality: i32: %d,\n", quality_score);
            fossil_io_printf("    passive_voice: i32: %d,\n", passive_ratio);
            fossil_io_printf("    style: cstr: \"%s\",\n", style ? style : "");
            fossil_io_printf("    tone: cstr: \"%s\"\n", tone ? tone : "");
            fossil_io_printf("  },\n");
        }

        if (extract_keywords) {
            fossil_io_printf("  keywords: array: [");
            for (int i = 0; i < kw_used; i++) {
                fossil_io_printf("cstr: \"%s\"", keywords[i].word);
                if (i + 1 < kw_used) fossil_io_printf(", ");
            }
            fossil_io_printf("],\n");
        }

        if (extract_topics) {
            fossil_io_printf("  topics: array: [cstr: \"group-1\", cstr: \"group-2\"],\n");
        }

        fossil_io_printf("  summary: cstr: \"%s\",\n", summary ? summary : "");
        fossil_io_printf("  key_sentence: cstr: \"%s\",\n", key_sentence ? key_sentence : "");

        fossil_io_printf("  flags: object: {\n");
        fossil_io_printf("    ragebait: bool: %s,\n", ragebait ? "true" : "false");
        fossil_io_printf("    clickbait: bool: %s,\n", clickbait ? "true" : "false");
        fossil_io_printf("    spam: bool: %s,\n", spam ? "true" : "false");
        fossil_io_printf("    woke: bool: %s,\n", woke ? "true" : "false");
        fossil_io_printf("    bot: bool: %s,\n", bot ? "true" : "false");
        fossil_io_printf("    sarcasm: bool: %s,\n", sarcasm ? "true" : "false");
        fossil_io_printf("    formal: bool: %s,\n", formal ? "true" : "false");
        fossil_io_printf("    snowflake: bool: %s,\n", snowflake ? "true" : "false");
        fossil_io_printf("    offensive: bool: %s,\n", offensive ? "true" : "false");
        fossil_io_printf("    neutral: bool: %s,\n", neutral ? "true" : "false");
        fossil_io_printf("    hype: bool: %s,\n", hype ? "true" : "false");
        fossil_io_printf("    quality: bool: %s,\n", quality ? "true" : "false");
        fossil_io_printf("    political: bool: %s,\n", political ? "true" : "false");
        fossil_io_printf("    conspiracy: bool: %s,\n", conspiracy ? "true" : "false");
        fossil_io_printf("    marketing: bool: %s,\n", marketing ? "true" : "false");
        fossil_io_printf("    technobabble: bool: %s\n", technobabble ? "true" : "false");
        fossil_io_printf("  }\n");

        fossil_io_printf("}\n");
    }

    fossil_sys_memory_free(linebuf);
    fossil_sys_memory_free(entbuf);
    if (summary) fossil_sys_memory_free(summary);
    if (key_sentence) fossil_sys_memory_free(key_sentence);
    return 0;
}
