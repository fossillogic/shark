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
 * Uses markup tags for colorizing tokens: {red}, {green}, {yellow}, {blue}, {magenta}, {cyan}, {normal}, etc.
 */
int fossil_shark_view(ccstring path, bool number_lines,
                      bool number_non_blank, bool squeeze_blank,
                      int head_lines, int tail_lines, bool show_time) {

    if (path == cnull) {
        fossil_io_fprintf(FOSSIL_STDERR,
                          "{red}Error:{normal} File path must be specified.\n");
        return 1;
    }

    /* ---------------------------------------------------------------------
       Determine file type
       --------------------------------------------------------------------- */
    bool is_media = false, is_code = false, is_structured = false;
    const char *ext = strrchr(path, '.');

    if (ext) {
        if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0 ||
            strcmp(ext, ".png") == 0 || strcmp(ext, ".gif") == 0 ||
            strcmp(ext, ".mp3") == 0 || strcmp(ext, ".mp4") == 0 ||
            strcmp(ext, ".wav") == 0 || strcmp(ext, ".avi") == 0 ||
            strcmp(ext, ".mov") == 0 || strcmp(ext, ".flac") == 0)
            is_media = true;

        else if (strcmp(ext, ".json") == 0 || strcmp(ext, ".xml") == 0 ||
                 strcmp(ext, ".yaml") == 0 || strcmp(ext, ".yml") == 0 ||
                 strcmp(ext, ".toml") == 0 || strcmp(ext, ".ini") == 0 ||
                 strcmp(ext, ".csv") == 0)
            is_structured = true;

        else if (strcmp(ext, ".c") == 0 || strcmp(ext, ".cpp") == 0 ||
                 strcmp(ext, ".h") == 0 || strcmp(ext, ".hpp") == 0 ||
                 strcmp(ext, ".py") == 0 || strcmp(ext, ".js") == 0 ||
                 strcmp(ext, ".java") == 0 || strcmp(ext, ".cs") == 0 ||
                 strcmp(ext, ".go") == 0 || strcmp(ext, ".rs") == 0 ||
                 strcmp(ext, ".sh") == 0 || strcmp(ext, ".html") == 0 ||
                 strcmp(ext, ".css") == 0)
            is_code = true;
    }

    /* ---------------------------------------------------------------------
       Open the file
       --------------------------------------------------------------------- */
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, path, "r") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red}Error:{normal} ");
        perror(path);
        return 1;
    }

    /* ---------------------------------------------------------------------
       Show timestamps (optional)
       --------------------------------------------------------------------- */
    if (show_time) {
#ifdef _WIN32
        struct _stat st;
        if (_stat(path, &st) == 0) {
            fossil_io_printf("{blue}File:{normal} %s\n", path);
            fossil_io_printf("{blue}Size:{normal} %ld bytes\n", (long)st.st_size);
            char time_buf[64];
            struct tm *tm_info = localtime(&st.st_mtime);
            if (tm_info) {
                strftime(time_buf, sizeof(time_buf), "%c", tm_info);
                fossil_io_printf("{blue}Modified:{normal} %s\n", time_buf);
            }
        }
#else
        struct stat st;
        if (stat(path, &st) == 0) {
            fossil_io_printf("{blue}File:{normal} %s\n", path);
            fossil_io_printf("{blue}Size:{normal} %ld bytes\n", (long)st.st_size);
            fossil_io_printf("{blue}Modified:{normal} %s", ctime(&st.st_mtime));
        }
#endif
    }

    /* ---------------------------------------------------------------------
       Tail-mode buffering
       --------------------------------------------------------------------- */
    cstring *lines = cnull;
    size_t count = 0, capacity = 0;
    char buffer[8192];

    while (fossil_io_gets_from_stream(buffer, sizeof(buffer), &stream)) {

        fossil_io_trim(buffer);

        if (squeeze_blank && strlen(buffer) == 0) {
            if (count > 0 && cnotnull(lines) && strlen(lines[count - 1]) == 0)
                continue;
        }

        /* -----------------------------------------------------------------
           Enhanced whitespace + syntax colorizer
           ----------------------------------------------------------------- */
        char colored_buf[8192 * 4];
        size_t j = 0;

        for (size_t i = 0;
             buffer[i] != '\0' && j < sizeof(colored_buf) - 64; ++i) {

            unsigned char c = buffer[i];

            /* ----- (1) Visible WHITESPACE ----- */
            if (c == ' ') {
                j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                              "{normal}{dim}·{normal}");
                continue;
            }

            if (c == '\t') {
                j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                              "{cyan}{dim}→{normal}");
                continue;
            }

            /* ----- (2) Media: everything magenta ----- */
            if (is_media) {
                j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                              "{magenta}%c{normal}", c);
                continue;
            }

            /* ----- (3) Structured formats ----- */
            if (is_structured) {
                if (strchr("{}[]:,\"'", c)) {
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{cyan}%c{normal}", c);
                    continue;
                }

                if ((i == 0 ||
                    buffer[i - 1] == '"' ||
                    buffer[i - 1] == '\'') &&
                    isalpha(c))
                {
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{green}%c{normal}", c);
                    continue;
                }

                if (isdigit(c)) {
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{yellow}%c{normal}", c);
                    continue;
                }

                j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                              "{normal}%c", c);
                continue;
            }

            /* ----- (4) Code files ----- */
            if (is_code) {

                /* Inline comment */
                if (c == '/' && buffer[i + 1] == '/') {
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{green}//%s{normal}", buffer + i + 2);
                    break;
                }

                /* Operators / punctuation */
                if (strchr("{}()[];,+-*/%<>&|^!=", c)) {
                    j += snprintf(colored_buf + j,
                                  sizeof(colored_buf) - j,
                                  "{yellow}%c{normal}", c);
                    continue;
                }

                /* Keywords / identifiers */
                if (isalpha(c) || c == '_') {
                    char tok[128];
                    size_t t = 0;
                    size_t start = i;

                    while ((isalnum(buffer[i]) || buffer[i] == '_') &&
                           t < sizeof(tok) - 1) {
                        tok[t++] = buffer[i];
                        i++;
                    }
                    tok[t] = '\0';
                    i--;

                    const char *keywords[] = {
                        "if","else","for","while","return","struct","typedef",
                        "enum","void","int","float","double","char","const",
                        "static","switch","case","break","continue","union",
                        "goto","sizeof"
                    };

                    int is_kw = 0;
                    for (size_t k = 0;
                         k < sizeof(keywords)/sizeof(keywords[0]); ++k) {
                        if (strcmp(tok, keywords[k]) == 0) {
                            j += snprintf(colored_buf + j,
                                          sizeof(colored_buf) - j,
                                          "{blue}%s{normal}", tok);
                            is_kw = 1;
                            break;
                        }
                    }

                    if (!is_kw) {
                        j += snprintf(colored_buf + j,
                                      sizeof(colored_buf) - j,
                                      "{normal}%s", tok);
                    }

                    continue;
                }

                /* Normal code character */
                j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                              "{normal}%c", c);
                continue;
            }

            /* ----- (5) Default category ----- */
            j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                          "{green}%c{normal}", c);
        }

        colored_buf[j] = '\0';

        /* -----------------------------------------------------------------
           Output or buffer for tail
           ----------------------------------------------------------------- */
        if (tail_lines > 0) {
            if (count >= capacity) {
                capacity = capacity ? capacity * 2 : 128;
                cstring *nl =
                    (cstring *)fossil_sys_memory_realloc(
                        lines, capacity * sizeof(cstring));
                if (nl == cnull) cpanic("alloc fail");
                lines = nl;
            }
            lines[count++] =
                fossil_io_cstring_dup_safe(colored_buf, strlen(colored_buf));
        } else {
            if (head_lines == 0 || (int)count < head_lines) {
                if (number_lines)
                    fossil_io_printf("{blue}%6zu{normal}\t%s\n",
                                     count + 1, colored_buf);
                else if (number_non_blank && strlen(buffer) > 0)
                    fossil_io_printf("{blue}%6zu{normal}\t%s\n",
                                     count + 1, colored_buf);
                else
                    fossil_io_printf("%s\n", colored_buf);
            }
            count++;
            if (head_lines > 0 && (int)count >= head_lines) break;
        }
    }

    fossil_fstream_close(&stream);

    /* ---------------------------------------------------------------------
       Tail output
       --------------------------------------------------------------------- */
    if (tail_lines > 0 && count > 0 && cnotnull(lines)) {
        size_t start = count > (size_t)tail_lines ?
                       count - tail_lines : 0;

        for (size_t i = start; i < count; i++) {
            if (number_lines)
                fossil_io_printf("{blue}%6zu{normal}\t%s\n",
                                 i + 1, lines[i]);
            else if (number_non_blank && strlen(lines[i]) > 0)
                fossil_io_printf("{blue}%6zu{normal}\t%s\n",
                                 i + 1, lines[i]);
            else
                fossil_io_printf("%s\n", lines[i]);

            fossil_io_cstring_free_safe(&lines[i]);
        }
        fossil_sys_memory_free(lines);
        cnullify(lines);
    }

    return 0;
}
