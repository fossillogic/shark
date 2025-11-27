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


static double summary_calc_entropy(const unsigned char *buf, size_t len) {
    if (len == 0) return 0.0;

    int freq[256] = {0};
    for (size_t i = 0; i < len; i++) freq[buf[i]]++;

    double entropy = 0.0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] == 0) continue;
        double p = (double)freq[i] / (double)len;
        entropy -= p * log2(p);
    }
    return entropy;
}

static ccstring summary_detect_type(ccstring path) {
    size_t len = strlen(path);
    if (len >= 5 && strcmp(path + len - 4, ".log") == 0) return "log";
    if (len >= 4 && strcmp(path + len - 3, ".md") == 0) return "markdown";
    if (len >= 5 && strcmp(path + len - 4, ".txt") == 0) return "text";
    if (len >= 4 && strcmp(path + len - 3, ".c") == 0) return "c-code";
    if (len >= 3 && strcmp(path + len - 2, ".h") == 0) return "c-header";
    return "text"; // safe fallback
}

static char **summary_extract_keywords(const char *text, int *out_count) {
    *out_count = 0;
    if (!text) return NULL;

    // Very naive: split by whitespace, frequency count
    char *copy = fossil_sys_memory_strdup(text);
    if (!copy) return NULL;

    // Temporary map
    struct {
        char *word;
        int count;
    } table[256];

    int used = 0;
    char *tok = strtok(copy, " \t\r\n.,:;(){}[]<>!?\"'");
    while (tok && used < 256) {
        // ignore tiny words
        if (strlen(tok) > 3) {
            // find existing
            int found = -1;
            for (int i = 0; i < used; i++) {
                if (strcmp(table[i].word, tok) == 0) {
                    found = i;
                    break;
                }
            }
            if (found >= 0) table[found].count++;
            else {
                table[used].word = fossil_sys_memory_strdup(tok);
                table[used].count = 1;
                used++;
            }
        }
        tok = strtok(NULL, " \t\r\n.,:;(){}[]<>!?\"'");
    }

    // Sort by frequency descending
    for (int i = 0; i < used; i++)
        for (int j = i + 1; j < used; j++)
            if (table[j].count > table[i].count) {
                typeof(table[0]) tmp = table[i];
                table[i] = table[j];
                table[j] = tmp;
            }

    // return top 20
    int out_n = used < 20 ? used : 20;
    char **result = fossil_sys_memory_alloc(sizeof(char*) * out_n);
    for (int i = 0; i < out_n; i++)
        result[i] = fossil_sys_memory_strdup(table[i].word);

    for (int i = 0; i < used; i++)
        fossil_sys_memory_free(table[i].word);
    fossil_sys_memory_free(copy);

    *out_count = out_n;
    return result;
}

int fossil_shark_summary(const char **paths, int count,
                         int max_lines, bool auto_detect,
                         bool do_keywords, bool do_topics,
                         bool do_stats, bool fson)
{
    if (!paths || count == 0)
        return EINVAL;

    for (int i = 0; i < count; i++) {
        ccstring path = paths[i];

        fossil_io_printf("{blue}=== SUMMARY: %s ==={normal}\n", path);

        FILE *fp = fopen(path, "rb");
        if (!fp) {
            fossil_io_fprintf(FOSSIL_STDERR, "Cannot open %s\n", path);
            continue;
        }

        // Read file with line limit
        char *buffer = fossil_sys_memory_alloc(1024 * 1024);
        size_t total = 0;
        size_t cap = 1024 * 1024;
        int lines = 0;

        while (!feof(fp)) {
            if (max_lines && lines >= max_lines) break;

            char line[4096];
            if (!fgets(line, sizeof(line), fp)) break;

            size_t len = strlen(line);
            if (total + len + 1 >= cap) break;

            memcpy(buffer + total, line, len);
            total += len;
            buffer[total] = '\0';

            lines++;
        }

        fclose(fp);

        // Auto detect type
        const char *ftype = auto_detect ? summary_detect_type(path)
                                        : "raw";

        // Stats block
        size_t char_count = total;
        double entropy = 0.0;
        if (do_stats && total > 0)
            entropy = summary_calc_entropy((unsigned char*)buffer, total);

        // Keywords
        int kw_count = 0;
        char **keywords = NULL;
        if (do_keywords) {
            keywords = summary_extract_keywords(buffer, &kw_count);
        }

        // Topics
        int topic_groups = 0;
        if (do_topics && kw_count > 0) {
            summary_topic_cluster(keywords, kw_count, &topic_groups);
        }

        // Output
        if (!fson) {
            // Regular human readable
            fossil_io_printf("{blue}Type:{normal} %s\n", ftype);

            if (do_stats) {
                fossil_io_printf("{blue}Lines:{normal} %d\n", lines);
                fossil_io_printf("{blue}Chars:{normal} %zu\n", char_count);
                fossil_io_printf("{blue}Entropy:{normal} %.3f\n", entropy);
            }

            if (do_keywords) {
                fossil_io_printf("{blue}Keywords:{normal} ");
                for (int k = 0; k < kw_count; k++)
                    fossil_io_printf("%s ", keywords[k]);
                fossil_io_printf("\n");
            }

            if (do_topics) {
                fossil_io_printf("{blue}Topic Groups:{normal} %d\n",
                                 topic_groups);
            }
        } else {
            // FSON Output
            fossil_io_printf("{blue}{\n{normal}");
            fossil_io_printf("  \"file\": \"%s\",\n", path);
            fossil_io_printf("  \"type\": \"%s\",\n", ftype);
            fossil_io_printf("  \"lines\": %d,\n", lines);
            fossil_io_printf("  \"chars\": %zu,\n", char_count);
            if (do_stats)
                fossil_io_printf("  \"entropy\": %.3f,\n", entropy);

            if (do_keywords) {
                fossil_io_printf("  \"keywords\": [");
                for (int k = 0; k < kw_count; k++) {
                    fossil_io_printf("\"%s\"", keywords[k]);
                    if (k + 1 < kw_count) fossil_io_printf(", ");
                }
                fossil_io_printf("],\n");
            }

            if (do_topics)
                fossil_io_printf("  \"topic_groups\": %d\n", topic_groups);

            fossil_io_printf("{blue}}{normal}\n");
        }

        // Cleanup
        if (keywords) {
            for (int k = 0; k < kw_count; k++)
                fossil_sys_memory_free(keywords[k]);
            fossil_sys_memory_free(keywords);
        }

        fossil_sys_memory_free(buffer);
    }

    return 0;
}
