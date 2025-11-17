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
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

/* ---------------------------------------------------------------------
   Preloaded language/token tables (as requested)
   --------------------------------------------------------------------- */
static const char *KW_CODE[] = {
    // Control flow
    "if","else","for","while","do","switch","case","default",
    "break","continue","goto","return",

    // Types
    "void","char","short","int","long","float","double","signed","unsigned",
    "size_t","ssize_t","ptrdiff_t","struct","union","enum","typedef",
    "class","interface","fn","let","var","const","static","extern",

    // Modern languages
    "async","await","yield","lambda","import","from","package","module",

    // Rust/Go
    "fn","pub","crate","impl","use","match","const","let",

    // JS / TS
    "function","export","extends","new","this","super",

    // Python
    "def","pass","class","self","with","as","try","except","finally","raise",
};

static const char OPS_CODE[] =
    "{}()[];,+-*/%<>&|^!=~?:.";

static const char *COMMENTS_CODE[] = {
    "//",  /* C, C++, JS, Java, Rust, Go */
    "#",   /* Python, shell */
    "/*",  /* C-style block open */
    "*/",  /* C-style block close */
};

static const char STR_PUNCT[] = "{}[]:,\"'=<>/";

static const char *KW_STRUCT[] = {
    "true","false","null","yes","no","on","off",
};

#define is_number_char(c) (isdigit((unsigned char)(c)) || (c)=='-' || (c)=='.')

/* ---------------------------------------------------------------------
   Helper: check keyword list (linear scan). Could be optimized later.
   --------------------------------------------------------------------- */
static int is_in_table(const char *tok, const char *table[], size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (table[i] == NULL) continue;
        if (strcmp(tok, table[i]) == 0) return 1;
    }
    return 0;
}

/* ---------------------------------------------------------------------
   The main function (integrated)
   --------------------------------------------------------------------- */
int fossil_shark_view(ccstring path, bool number_lines,
                      bool number_non_blank, bool squeeze_blank,
                      int head_lines, int tail_lines, bool show_time) {

    if (path == cnull) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red}Error:{normal} File path must be specified.\n");
        return 1;
    }

    /* Determine file type by extension */
    bool is_media = false, is_code = false, is_structured = false;
    const char *ext = strrchr(path, '.');
    if (ext) {
        if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0 ||
            strcmp(ext, ".png") == 0 || strcmp(ext, ".gif") == 0 ||
            strcmp(ext, ".mp3") == 0 || strcmp(ext, ".mp4") == 0 ||
            strcmp(ext, ".wav") == 0 || strcmp(ext, ".avi") == 0 ||
            strcmp(ext, ".mov") == 0 || strcmp(ext, ".flac") == 0) {
            is_media = true;
        } else if (strcmp(ext, ".json") == 0 || strcmp(ext, ".xml") == 0 ||
                   strcmp(ext, ".yaml") == 0 || strcmp(ext, ".yml") == 0 ||
                   strcmp(ext, ".toml") == 0 || strcmp(ext, ".ini") == 0 ||
                   strcmp(ext, ".csv") == 0) {
            is_structured = true;
        } else if (strcmp(ext, ".c") == 0 || strcmp(ext, ".cpp") == 0 ||
                   strcmp(ext, ".h") == 0 || strcmp(ext, ".hpp") == 0 ||
                   strcmp(ext, ".py") == 0 || strcmp(ext, ".js") == 0 ||
                   strcmp(ext, ".java") == 0 || strcmp(ext, ".cs") == 0 ||
                   strcmp(ext, ".go") == 0 || strcmp(ext, ".rs") == 0 ||
                   strcmp(ext, ".sh") == 0 || strcmp(ext, ".html") == 0 ||
                   strcmp(ext, ".css") == 0) {
            is_code = true;
        }
    }

    /* Open file */
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, path, "r") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red}Error:{normal} ");
        perror(path);
        return 1;
    }

    /* Optional timestamp info */
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

    /* Tail buffering support */
    cstring *lines = cnull;
    size_t count = 0, capacity = 0;
    char buffer[8192];

    while (fossil_io_gets_from_stream(buffer, sizeof(buffer), &stream)) {

        /* Trim right newline but preserve leading/trailing spaces (we visualize them) */
        fossil_io_trim(buffer);

        if (squeeze_blank && strlen(buffer) == 0) {
            if (count > 0 && cnotnull(lines) && strlen(lines[count - 1]) == 0) continue;
        }

        /* Enhanced token-aware colorization */
        char colored_buf[8192 * 4];
        size_t j = 0;

        for (size_t i = 0; buffer[i] != '\0' && j < sizeof(colored_buf) - 128; ++i) {
            unsigned char c = (unsigned char)buffer[i];

            /* -------------------------
               (A) Visual whitespace
               ------------------------- */
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

            /* -------------------------
               (B) Media files: magenta everything
               ------------------------- */
            if (is_media) {
                j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                              "{magenta}%c{normal}", c);
                continue;
            }

            /* -------------------------
               (C) Structured formats: JSON/YAML/TOML/INI/CSV
               - punctuation -> cyan
               - boolean/null -> green,bold
               - numbers -> yellow,bold
               - default -> normal
               ------------------------- */
            if (is_structured) {
                if (strchr(STR_PUNCT, c)) {
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{cyan}%c{normal}", c);
                    continue;
                }

                /* Simple token scans for keywords (true/false/null/etc.) */
                if (isalpha(c)) {
                    char tok[128];
                    size_t t = 0;
                    while ((isalnum((unsigned char)buffer[i]) || buffer[i] == '_') && t < sizeof(tok)-1) {
                        tok[t++] = buffer[i];
                        i++;
                    }
                    tok[t] = '\0';
                    i--; /* rewind one char as main loop will advance */

                    if (is_in_table(tok, KW_STRUCT, sizeof(KW_STRUCT)/sizeof(KW_STRUCT[0]))) {
                        j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                      "{green,bold}%s{normal}", tok);
                    } else {
                        j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                      "{normal}%s", tok);
                    }
                    continue;
                }

                /* Numbers */
                if (is_number_char(c)) {
                    size_t k = i;
                    char numtok[64];
                    size_t nk = 0;
                    while (buffer[k] != '\0' && is_number_char(buffer[k]) && nk < sizeof(numtok)-1) {
                        numtok[nk++] = buffer[k++];
                    }
                    numtok[nk] = '\0';
                    /* advance main index */
                    i = k - 1;
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{yellow,bold}%s{normal}", numtok);
                    continue;
                }

                /* fallback */
                j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                              "{normal}%c", c);
                continue;
            }

            /* -------------------------
               (D) Code files: keywords, operators, comments, strings, numbers
               ------------------------- */
            if (is_code) {

                /* (1) Comments: // and # - rest of line; /* ... * / handled too */
                if (c == '/' && buffer[i+1] == '/') {
                    /* print '//' and the rest as green */
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{green}//%s{normal}", buffer + i + 2);
                    break; /* rest of line handled */
                }
                if (c == '#') {
                    /* shell/Python-style comment: color rest of line */
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{green}#%s{normal}", buffer + i + 1);
                    break;
                }
                if (c == '/' && buffer[i+1] == '*') {
                    /* C-style block comment: find closing '* /' or end-of-line */
                    size_t k = i + 2;
                    while (buffer[k] != '\0') {
                        if (buffer[k] == '*' && buffer[k+1] == '/') {
                            k += 2;
                            break;
                        }
                        k++;
                    }
                    /* Print the comment region green */
                    size_t len = k - i;
                    char *chunk = (char *)fossil_sys_memory_alloc(len + 1);
                    if (chunk) {
                        memcpy(chunk, buffer + i, len);
                        chunk[len] = '\0';
                        j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                      "{green}%s{normal}", chunk);
                        fossil_sys_memory_free(chunk);
                    } else {
                        /* fallback if alloc fails */
                        j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                      "{green}%c{normal}", c);
                    }
                    i = k - 1;
                    continue;
                }

                /* (2) Strings: "..." or '...' - punctuation cyan, contents magenta */
                if (c == '"' || c == '\'') {
                    char q = c;
                    /* opening quote punctuation */
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{cyan}%c{normal}", q);
                    size_t k = i + 1;
                    while (buffer[k] != '\0') {
                        if (buffer[k] == '\\' && buffer[k+1] != '\0') {
                            /* escape sequence: color the escape and the next char as magenta */
                            char esc[3] = { buffer[k], buffer[k+1], '\0' };
                            j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                          "{magenta}%s{normal}", esc);
                            k += 2;
                            continue;
                        }
                        if (buffer[k] == q) {
                            /* closing quote punctuation */
                            j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                          "{cyan}%c{normal}", q);
                            break;
                        }
                        /* interior string content */
                        /* group contiguous runs for efficiency */
                        size_t run_start = k;
                        while (buffer[k] != '\0' && buffer[k] != q && buffer[k] != '\\') k++;
                        size_t run_len = k - run_start;
                        if (run_len > 0) {
                            /* copy run to temporary to print as magenta */
                            char *run = (char *)fossil_sys_memory_alloc(run_len + 1);
                            if (run) {
                                memcpy(run, buffer + run_start, run_len);
                                run[run_len] = '\0';
                                j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                              "{magenta}%s{normal}", run);
                                fossil_sys_memory_free(run);
                            } else {
                                /* fallback char-by-char */
                                for (size_t z = run_start; z < run_start + run_len; ++z) {
                                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                                  "{magenta}%c{normal}", buffer[z]);
                                }
                            }
                        }
                        /* if loop breaks due to closing quote, the loop will handle it */
                    }
                    /* move main index to position of closing quote (or end) */
                    while (buffer[i+1] != '\0' && buffer[i+1] != q) i++;
                    if (buffer[i+1] == q) i++; /* skip closing quote, already printed above */
                    continue;
                }

                /* (3) Operators & punctuation */
                if (strchr(OPS_CODE, c)) {
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{yellow}%c{normal}", c);
                    continue;
                }
                if (strchr(STR_PUNCT, c)) {
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{cyan}%c{normal}", c);
                    continue;
                }

                /* (4) Numbers: digit or -/.\ leading */
                if (is_number_char(c)) {
                    size_t k = i;
                    char numtok[128];
                    size_t nk = 0;
                    /* Collect a pragmatic number token: digits, dot, sign, exponent letters not handled exhaustively */
                    while (buffer[k] != '\0' && (isdigit((unsigned char)buffer[k]) || buffer[k] == '.' || buffer[k] == '-' || buffer[k] == '+' || buffer[k] == 'e' || buffer[k] == 'E') && nk < sizeof(numtok)-1) {
                        numtok[nk++] = buffer[k++];
                    }
                    numtok[nk] = '\0';
                    i = k - 1;
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{yellow,bold}%s{normal}", numtok);
                    continue;
                }

                /* (5) Identifiers / keywords */
                if (isalpha(c) || c == '_') {
                    char tok[256];
                    size_t t = 0;
                    size_t k = i;
                    while ((isalnum((unsigned char)buffer[k]) || buffer[k] == '_') && t < sizeof(tok)-1) {
                        tok[t++] = buffer[k++];
                    }
                    tok[t] = '\0';
                    i = k - 1;

                    /* Check KW_CODE */
                    if (is_in_table(tok, KW_CODE, sizeof(KW_CODE)/sizeof(KW_CODE[0]))) {
                        j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                      "{blue}%s{normal}", tok);
                        continue;
                    }
                    /* Structured word check (true/false/null etc) */
                    if (is_in_table(tok, KW_STRUCT, sizeof(KW_STRUCT)/sizeof(KW_STRUCT[0]))) {
                        j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                      "{green,bold}%s{normal}", tok);
                        continue;
                    }

                    /* default identifier */
                    j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                                  "{normal}%s", tok);
                    continue;
                }

                /* fallback single char */
                j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                              "{normal}%c", c);
                continue;
            } /* end is_code */

            /* -------------------------
               (E) Default: color green
               ------------------------- */
            j += snprintf(colored_buf + j, sizeof(colored_buf) - j,
                          "{green}%c{normal}", c);
        } /* end for */

        colored_buf[j] = '\0';

        /* Output or buffer for tail-lines */
        if (tail_lines > 0) {
            if (count >= capacity) {
                capacity = capacity ? capacity * 2 : 128;
                cstring *new_lines;
                if (lines == cnull) {
                    new_lines = (cstring *)fossil_sys_memory_alloc(capacity * sizeof(cstring));
                } else {
                    new_lines = (cstring *)fossil_sys_memory_realloc(lines, capacity * sizeof(cstring));
                }
                if (new_lines == cnull) cpanic("Memory allocation failed for lines buffer");
                lines = new_lines;
            }
            lines[count++] = fossil_io_cstring_dup_safe(colored_buf, strlen(colored_buf));
        } else {
            if (head_lines == 0 || (int)count < head_lines) {
                if (number_lines)
                    fossil_io_printf("{blue}%6zu{normal}\t%s\n", count + 1, colored_buf);
                else if (number_non_blank && strlen(buffer) > 0)
                    fossil_io_printf("{blue}%6zu{normal}\t%s\n", count + 1, colored_buf);
                else
                    fossil_io_printf("%s\n", colored_buf);
            }
            count++;
            if (head_lines > 0 && (int)count >= head_lines) break;
        }
    } /* end while */

    fossil_fstream_close(&stream);

    /* Tail output flush */
    if (tail_lines > 0 && count > 0 && cnotnull(lines)) {
        size_t start = count > (size_t)tail_lines ? count - tail_lines : 0;
        for (size_t i = start; i < count; i++) {
            if (number_lines)
                fossil_io_printf("{blue}%6zu{normal}\t%s\n", i + 1, lines[i]);
            else if (number_non_blank && strlen(lines[i]) > 0)
                fossil_io_printf("{blue}%6zu{normal}\t%s\n", i + 1, lines[i]);
            else
                fossil_io_printf("%s\n", lines[i]);
            fossil_io_cstring_free_safe(&lines[i]);
        }
        fossil_sys_memory_free(lines);
        cnullify(lines);
    }

    return 0;
}
