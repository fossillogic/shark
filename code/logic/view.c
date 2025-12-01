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


/* -----------------------------------------------------------------------
    Utility: detect if file is binary
    ----------------------------------------------------------------------- */
static bool is_binary_file(const char *path) {
     fossil_io_file_t stream;
     if (fossil_io_file_open(&stream, path, "rb") != 0)
          return false;

     unsigned char buf[256];
     size_t n = fossil_io_file_read(&stream, buf, 1, sizeof(buf));
     fossil_io_file_close(&stream);

     for (size_t i = 0; i < n; i++) {
          if (buf[i] == 0) return true;
     }
     return false;
}

/* -----------------------------------------------------------------------
    Utility: determine type from extension
    ----------------------------------------------------------------------- */
static const char *get_extension(const char *path) {
     const char *dot = strrchr(path, '.');
     return dot ? dot + 1 : "";
}

static bool is_code_ext(const char *ext) {
     const char *list[] = {
          "c","h","cpp","hpp","cc","java","js","ts","py","go","rs","lua",
          "php","rb","swift","cs","m","mm","sh"
     };
     for (size_t i = 0; i < sizeof(list)/sizeof(list[0]); i++)
          if (fossil_io_cstring_iequals(ext, list[i]))
                return true;
     return false;
}

static bool is_structured_ext(ccstring ext) {
    const ccstring list[] = { "json", "xml", "yaml", "yml", "ini", "toml" };
    for (size_t i = 0; i < sizeof(list)/sizeof(list[0]); i++)
        if (fossil_io_cstring_iequals(ext, list[i]))
             return true;
    return false;
}

static bool is_media_ext(ccstring ext) {
    const ccstring list[] = {
        "jpg","jpeg","png","gif","bmp","mp3","wav","ogg","flac","mp4",
        "mkv","avi","webm"
    };
    for (size_t i = 0; i < sizeof(list)/sizeof(list[0]); i++)
        if (fossil_io_cstring_iequals(ext, list[i]))
             return true;
    return false;
}


/* -----------------------------------------------------------------------
    Code formatting (very simple keyword highlighter)
    ----------------------------------------------------------------------- */
// --- Advanced code keyword lists ---
static const char *KW_CODE[] = {
    // Control
    "if","else","for","while","do","switch","case","default","break",
    "continue","goto","return",
    // Types
    "void","char","short","int","long","float","double","signed","unsigned",
    "struct","union","enum","typedef","const","volatile","static","extern","register",
    // Modifiers
    "inline","restrict","auto","sizeof",
    // C++
    "class","public","private","protected","virtual","override","template","typename",
    "namespace","using","new","delete","operator","friend","this","throw","try","catch",
    "const_cast","static_cast","dynamic_cast","reinterpret_cast",
    // C99/C11/C++11
    "bool","true","false","nullptr","constexpr","decltype","thread_local","noexcept",
    // Preprocessor
    "#include","#define","#ifdef","#ifndef","#endif","#if","#else","#elif","#undef","#pragma",
    // Misc
    "switch","case","default","break","continue","return",
    NULL
};

static const char *KW_CODE_TYPES[] = {
    "void","char","short","int","long","float","double","signed","unsigned",
    "struct","union","enum","typedef","const","volatile","static","extern","register",
    "bool","size_t","ssize_t","uint8_t","uint16_t","uint32_t","uint64_t",
    "int8_t","int16_t","int32_t","int64_t", NULL
};

static const char *KW_CODE_PREPROC[] = {
    "#include","#define","#ifdef","#ifndef","#endif","#if","#else","#elif","#undef","#pragma", NULL
};

static const char *KW_CODE_LITERAL[] = {
    "NULL","nullptr","true","false", NULL
};

static bool is_code_keyword(ccstring word) {
    for (int i = 0; KW_CODE[i]; i++)
        if (fossil_io_cstring_iequals(word, KW_CODE[i]))
            return true;
    return false;
}
static bool is_code_type(ccstring word) {
    for (int i = 0; KW_CODE_TYPES[i]; i++)
        if (fossil_io_cstring_iequals(word, KW_CODE_TYPES[i]))
            return true;
    return false;
}
static bool is_code_preproc(ccstring word) {
    for (int i = 0; KW_CODE_PREPROC[i]; i++)
        if (fossil_io_cstring_iequals(word, KW_CODE_PREPROC[i]))
            return true;
    return false;
}
static bool is_code_literal(ccstring word) {
    for (int i = 0; KW_CODE_LITERAL[i]; i++)
        if (fossil_io_cstring_iequals(word, KW_CODE_LITERAL[i]))
            return true;
    return false;
}

// --- Advanced code formatter ---
static void format_code_line(ccstring line) {
    size_t len = fossil_io_cstring_length(line);
    size_t i = 0;
    bool in_string = false, in_char = false, in_multi_comment = false;

    // Indentation
    size_t indent_len = 0;
    while (i < len && (line[i] == ' ' || line[i] == '\t')) {
        indent_len++;
        i++;
    }
    if (indent_len > 0)
        fossil_io_printf("%.*s", (int)indent_len, line);

    // Preprocessor directive at line start
    if (line[indent_len] == '#') {
        fossil_io_printf("{magenta,bold}");
        for (; i < len; i++) fossil_io_putchar(line[i]);
        fossil_io_printf("{normal}");
        return;
    }

    while (i < len) {
        // Single-line comment
        if (!in_string && !in_char && line[i] == '/' && i + 1 < len && line[i+1] == '/') {
            fossil_io_printf("{green}//%s{normal}", &line[i+2]);
            break;
        }
        // Multi-line comment start
        if (!in_string && !in_char && line[i] == '/' && i + 1 < len && line[i+1] == '*') {
            fossil_io_printf("{green}/*");
            i += 2;
            in_multi_comment = true;
            continue;
        }
        // Multi-line comment end
        if (in_multi_comment) {
            if (line[i] == '*' && i + 1 < len && line[i+1] == '/') {
                fossil_io_printf("*/{normal}");
                i += 2;
                in_multi_comment = false;
            } else {
                fossil_io_putchar(line[i++]);
            }
            continue;
        }
        // String literal
        if (!in_char && line[i] == '"' && !in_string) {
            fossil_io_printf("{yellow}\"");
            i++;
            in_string = true;
            while (i < len) {
                if (line[i] == '\\' && i + 1 < len) {
                    fossil_io_printf("\\%c", line[i+1]);
                    i += 2;
                    continue;
                }
                fossil_io_putchar(line[i]);
                if (line[i] == '"' && line[i-1] != '\\') {
                    fossil_io_printf("{normal}");
                    i++;
                    in_string = false;
                    break;
                }
                i++;
            }
            continue;
        }
        // Char literal
        if (!in_string && line[i] == '\'' && !in_char) {
            fossil_io_printf("{magenta}'");
            i++;
            in_char = true;
            while (i < len) {
                if (line[i] == '\\' && i + 1 < len) {
                    fossil_io_printf("\\%c", line[i+1]);
                    i += 2;
                    continue;
                }
                fossil_io_putchar(line[i]);
                if (line[i] == '\'' && line[i-1] != '\\') {
                    fossil_io_printf("{normal}");
                    i++;
                    in_char = false;
                    break;
                }
                i++;
            }
            continue;
        }
        // Numbers (hex, float, int)
        if (isdigit((unsigned char)line[i]) ||
            (line[i] == '-' && i + 1 < len && isdigit((unsigned char)line[i+1]))) {
            size_t start = i;
            if (line[i] == '-') i++;
            if (line[i] == '0' && i + 1 < len && (line[i+1] == 'x' || line[i+1] == 'X')) {
                i += 2;
                while (i < len && isxdigit((unsigned char)line[i])) i++;
            } else {
                while (i < len && (isdigit((unsigned char)line[i]) || line[i] == '.' || line[i] == 'e' || line[i] == 'E' || line[i] == '+' || line[i] == '-')) i++;
            }
            cstring num = fossil_io_cstring_substring(line, start, i - start);
            fossil_io_printf("{blue}%s{normal}", num);
            fossil_io_cstring_free(num);
            continue;
        }
        // Identifiers, keywords, types, literals, preprocessor
        if (isalpha((unsigned char)line[i]) || line[i] == '_') {
            size_t start = i;
            while (i < len && (isalnum((unsigned char)line[i]) || line[i] == '_')) i++;
            cstring word = fossil_io_cstring_substring(line, start, i - start);
            if (is_code_preproc(word))
                fossil_io_printf("{magenta,bold}%s{normal}", word);
            else if (is_code_keyword(word))
                fossil_io_printf("{cyan,bold}%s{normal}", word);
            else if (is_code_type(word))
                fossil_io_printf("{yellow,bold}%s{normal}", word);
            else if (is_code_literal(word))
                fossil_io_printf("{magenta,bold}%s{normal}", word);
            else
                fossil_io_printf("%s", word);
            fossil_io_cstring_free(word);
            continue;
        }
        // Operators and punctuation
        if (strchr("=+-*/%&|^~!<>?:;,()[]{}.", line[i])) {
            fossil_io_printf("{blue}%c{normal}", line[i++]);
            continue;
        }
        // Whitespace
        if (line[i] == ' ' || line[i] == '\t') {
            fossil_io_putchar(line[i++]);
            continue;
        }
        // Default
        fossil_io_putchar(line[i++]);
    }
}

/* -----------------------------------------------------------------------
    Structured data formatting (simple keyword highlighter for JSON/YAML/XML)
    ----------------------------------------------------------------------- */
static const char *KW_STRUCTURED[] = {
    "true", "false", "null", "yes", "no", "on", "off", "none", NULL
};

static bool is_structured_keyword(ccstring word) {
    for (int i = 0; KW_STRUCTURED[i]; i++) {
        if (fossil_io_cstring_iequals(word, KW_STRUCTURED[i]))
            return true;
    }
    return false;
}

static void format_structured_line(ccstring line, ccstring ext) {
    size_t len = fossil_io_cstring_length(line);
    size_t i = 0;
    // Count leading spaces/tabs for indent
    size_t indent_len = 0;
    while (i < len && (line[i] == ' ' || line[i] == '\t')) {
        indent_len++;
        i++;
    }
    if (indent_len > 0)
        fossil_io_printf("%.*s", (int)indent_len, line);

    bool in_string = false;
    char string_quote = 0;

    // If extension is empty, treat as plain text
    if (!ext || ext[0] == '\0') {
        fossil_io_puts(line);
        return;
    }

    // Markdown support
    if (fossil_io_cstring_iequals(ext, "md") || fossil_io_cstring_iequals(ext, "markdown")) {
        // Headings
        if (line[0] == '#') {
            int h = 0;
            while (i < len && line[i] == '#') { h++; i++; }
            while (i < len && (line[i] == ' ' || line[i] == '\t')) i++;
            fossil_io_printf("{cyan,bold}%.*s{normal} ", h, line);
            fossil_io_printf("{white,bold}%s{normal}", &line[i]);
            if (line[len-1] != '\n') fossil_io_putchar('\n');
            return;
        }
        // Lists
        if (line[i] == '-' || line[i] == '*' || line[i] == '+') {
            fossil_io_printf("{yellow,bold}%c{normal} ", line[i]);
            i++;
        }
        // Blockquote
        if (line[i] == '>') {
            fossil_io_printf("{magenta,bold}>%s{normal}", &line[i+1]);
            if (line[len-1] != '\n') fossil_io_putchar('\n');
            return;
        }
        // Code block
        if (fossil_io_cstring_starts_with(line, "```")) {
            fossil_io_printf("{blue,bold}%s{normal}", line);
            if (line[len-1] != '\n') fossil_io_putchar('\n');
            return;
        }
        // Inline code
        for (; i < len; i++) {
            if (line[i] == '`') {
                fossil_io_printf("{blue}`");
                i++;
                size_t start = i;
                while (i < len && line[i] != '`') i++;
                cstring code = fossil_io_cstring_substring(line, start, i - start);
                fossil_io_printf("%s", code);
                fossil_io_cstring_free(code);
                if (i < len && line[i] == '`') {
                    fossil_io_printf("`{normal}");
                }
            } else if (line[i] == '*' || line[i] == '_') {
                // Bold/italic
                int mark = line[i];
                int cnt = 0;
                while (i < len && line[i] == mark) { cnt++; i++; }
                size_t start = i;
                while (i < len && line[i] != mark) i++;
                cstring txt = fossil_io_cstring_substring(line, start, i - start);
                if (cnt == 2)
                    fossil_io_printf("{bold}%s{normal}", txt);
                else
                    fossil_io_printf("{italic}%s{normal}", txt);
                fossil_io_cstring_free(txt);
                while (i < len && line[i] == mark) i++;
            } else {
                fossil_io_putchar(line[i]);
            }
        }
        if (line[len-1] != '\n') fossil_io_putchar('\n');
        return;
    }

    // CSV support
    if (fossil_io_cstring_iequals(ext, "csv")) {
        while (i < len) {
            if (line[i] == '"') {
                fossil_io_printf("{yellow}\"");
                i++;
                size_t start = i;
                while (i < len && line[i] != '"') i++;
                cstring val = fossil_io_cstring_substring(line, start, i - start);
                fossil_io_printf("%s", val);
                fossil_io_cstring_free(val);
                if (i < len && line[i] == '"') {
                    fossil_io_printf("\"{normal}");
                    i++;
                }
            } else if (line[i] == ',') {
                fossil_io_printf("{blue},");
                i++;
            } else {
                fossil_io_putchar(line[i++]);
            }
        }
        if (line[len-1] != '\n') fossil_io_putchar('\n');
        return;
    }

    // HTML support
    if (fossil_io_cstring_iequals(ext, "html") || fossil_io_cstring_iequals(ext, "htm")) {
        while (i < len) {
            if (line[i] == '<') {
                fossil_io_printf("{yellow,bold}<");
                i++;
                size_t tag_start = i;
                while (i < len && (isalnum((unsigned char)line[i]) || line[i] == '-' || line[i] == '_')) i++;
                cstring tag = fossil_io_cstring_substring(line, tag_start, i - tag_start);
                fossil_io_printf("%s", tag);
                fossil_io_cstring_free(tag);
                // Attributes inside tag
                while (i < len && line[i] != '>') {
                    if (line[i] == '=') {
                        fossil_io_printf("{yellow}={normal}");
                        i++;
                    } else if (line[i] == '"' || line[i] == '\'') {
                        char quote = line[i++];
                        fossil_io_printf("{yellow}%c", quote);
                        size_t val_start = i;
                        while (i < len && line[i] != quote) i++;
                        cstring val = fossil_io_cstring_substring(line, val_start, i - val_start);
                        fossil_io_printf("%s", val);
                        fossil_io_cstring_free(val);
                        if (i < len && line[i] == quote) {
                            fossil_io_printf("%c{normal}", quote);
                            i++;
                        }
                    } else {
                        fossil_io_putchar(line[i++]);
                    }
                }
                if (i < len && line[i] == '>') {
                    fossil_io_printf("{yellow,bold}>");
                    i++;
                }
                continue;
            }
            fossil_io_putchar(line[i++]);
        }
        if (line[len-1] != '\n') fossil_io_putchar('\n');
        return;
    }

    // FSON support (like JSON, but allow comments and trailing commas)
    if (fossil_io_cstring_iequals(ext, "fson")) {
        while (i < len) {
            // Comments (support // and #)
            if ((line[i] == '/' && i + 1 < len && line[i+1] == '/') ||
            line[i] == '#') {
            fossil_io_printf("{green}%s{normal}", &line[i]);
            break;
            }
            // Strings (quoted values)
            if (!in_string && (line[i] == '"' || line[i] == '\'')) {
            in_string = true;
            string_quote = line[i];
            fossil_io_printf("{yellow}%c", line[i++]);
            size_t start = i;
            while (i < len && (line[i] != string_quote || (i > start && line[i-1] == '\\'))) i++;
            cstring str = fossil_io_cstring_substring(line, start, i - start);
            fossil_io_printf("%s", str);
            fossil_io_cstring_free(str);
            if (i < len && line[i] == string_quote) {
                fossil_io_printf("%c{normal}", line[i]);
                i++;
            }
            in_string = false;
            continue;
            }
            // Keys (before colon)
            if ((isalpha((unsigned char)line[i]) || line[i] == '_')) {
                size_t key_start = i;
                while (i < len && (isalnum((unsigned char)line[i]) || line[i] == '_')) i++;
                // If followed by colon, it's a key or type
                if (i < len && line[i] == ':') {
                    cstring key = fossil_io_cstring_substring(line, key_start, i - key_start);
                    // Highlight types (i32, cstr, bool, object, array, null, oct, hex, bin)
                    if (fossil_io_cstring_iequals(key, "i32") ||
                    fossil_io_cstring_iequals(key, "i64") ||
                    fossil_io_cstring_iequals(key, "cstr") ||
                    fossil_io_cstring_iequals(key, "bool") ||
                    fossil_io_cstring_iequals(key, "object") ||
                    fossil_io_cstring_iequals(key, "array") ||
                    fossil_io_cstring_iequals(key, "null") ||
                    fossil_io_cstring_iequals(key, "oct") ||
                    fossil_io_cstring_iequals(key, "hex") ||
                    fossil_io_cstring_iequals(key, "bin")) {
                    fossil_io_printf("{magenta,bold}%s{normal}", key);
                    } else {
                    fossil_io_printf("{cyan,bold}%s{normal}", key);
                    }
                    fossil_io_cstring_free(key);
                    fossil_io_printf("{yellow}:%{normal}", line[i]);
                    i++;
                    continue;
                }
            }
            // Numbers (support for decimal, hex, octal, binary)
            if (isdigit((unsigned char)line[i]) ||
            (line[i] == '-' && i + 1 < len && isdigit((unsigned char)line[i+1])) ||
            (line[i] == '0' && i + 1 < len &&
                (line[i+1] == 'x' || line[i+1] == 'X' ||
                 line[i+1] == 'o' || line[i+1] == 'O' ||
                 line[i+1] == 'b' || line[i+1] == 'B'))) {
            size_t start = i;
            if (line[i] == '-') i++;
            if (line[i] == '0' && i + 1 < len) {
                if (line[i+1] == 'x' || line[i+1] == 'X') {
                i += 2;
                while (i < len && isxdigit((unsigned char)line[i])) i++;
                } else if (line[i+1] == 'o' || line[i+1] == 'O') {
                i += 2;
                while (i < len && (line[i] >= '0' && line[i] <= '7')) i++;
                } else if (line[i+1] == 'b' || line[i+1] == 'B') {
                i += 2;
                while (i < len && (line[i] == '0' || line[i] == '1')) i++;
                } else {
                while (i < len && isdigit((unsigned char)line[i])) i++;
                }
            } else {
                while (i < len && (isdigit((unsigned char)line[i]) || line[i] == '.')) i++;
            }
            cstring num = fossil_io_cstring_substring(line, start, i - start);
            fossil_io_printf("{blue}%s{normal}", num);
            fossil_io_cstring_free(num);
            continue;
            }
            // Punctuation
            if (strchr("{}[]:,=", line[i])) {
            fossil_io_printf("{yellow}%c{normal}", line[i++]);
            continue;
            }
            // Structured keywords (true, false, null, yes, no, on, off, none)
            if (isalpha((unsigned char)line[i])) {
            size_t start = i;
            while (i < len && (isalnum((unsigned char)line[i]) || line[i] == '_')) i++;
            cstring word = fossil_io_cstring_substring(line, start, i - start);
            if (is_structured_keyword(word))
                fossil_io_printf("{magenta,bold}%s{normal}", word);
            else
                fossil_io_printf("%s", word);
            fossil_io_cstring_free(word);
            continue;
            }
            fossil_io_putchar(line[i++]);
        }
        if (line[len-1] != '\n') fossil_io_putchar('\n');
        return;
    }

    // JSON support
    if (fossil_io_cstring_iequals(ext, "json")) {
        while (i < len) {
            // Strings
            if (!in_string && (line[i] == '"' || line[i] == '\'')) {
                in_string = true;
                string_quote = line[i];
                fossil_io_printf("{yellow}%c", line[i++]);
                size_t start = i;
                while (i < len && (line[i] != string_quote || (i > start && line[i-1] == '\\'))) i++;
                cstring str = fossil_io_cstring_substring(line, start, i - start);
                fossil_io_printf("%s", str);
                fossil_io_cstring_free(str);
                if (i < len && line[i] == string_quote) {
                    fossil_io_printf("%c{normal}", line[i]);
                    i++;
                }
                in_string = false;
                continue;
            }
            // Keys
            if (line[i] == '"') {
                size_t key_start = ++i;
                while (i < len && line[i] != '"') i++;
                cstring key = fossil_io_cstring_substring(line, key_start, i - key_start);
                fossil_io_printf("{cyan,bold}\"%s\"{normal}", key);
                fossil_io_cstring_free(key);
                if (i < len && line[i] == '"') i++;
                if (i < len && line[i] == ':') {
                    fossil_io_printf("{yellow}:");
                    i++;
                }
                continue;
            }
            // Numbers
            if (isdigit((unsigned char)line[i]) ||
                (line[i] == '-' && i + 1 < len && isdigit((unsigned char)line[i + 1]))) {
                size_t start = i;
                if (line[i] == '-') i++;
                if (line[i] == '0' && i + 1 < len && (line[i+1] == 'x' || line[i+1] == 'X')) {
                    i += 2;
                    while (i < len && isxdigit((unsigned char)line[i])) i++;
                } else {
                    while (i < len && (isdigit((unsigned char)line[i]) || line[i] == '.')) i++;
                }
                cstring num = fossil_io_cstring_substring(line, start, i - start);
                fossil_io_printf("{blue}%s{normal}", num);
                fossil_io_cstring_free(num);
                continue;
            }
            // Punctuation
            if (strchr("{}[]:,=", line[i])) {
                fossil_io_printf("{yellow}%c{normal}", line[i++]);
                continue;
            }
            // Structured keywords
            if (isalpha((unsigned char)line[i])) {
                size_t start = i;
                while (i < len && (isalnum((unsigned char)line[i]) || line[i] == '_')) i++;
                cstring word = fossil_io_cstring_substring(line, start, i - start);
                if (is_structured_keyword(word))
                    fossil_io_printf("{magenta,bold}%s{normal}", word);
                else
                    fossil_io_printf("%s", word);
                fossil_io_cstring_free(word);
                continue;
            }
            fossil_io_putchar(line[i++]);
        }
        if (line[len-1] != '\n') fossil_io_putchar('\n');
        return;
    }

    // XML support
    if (fossil_io_cstring_iequals(ext, "xml")) {
        while (i < len) {
            if (line[i] == '<') {
                fossil_io_printf("{yellow,bold}<");
                i++;
                size_t tag_start = i;
                while (i < len && (isalnum((unsigned char)line[i]) || line[i] == '-' || line[i] == '_')) i++;
                cstring tag = fossil_io_cstring_substring(line, tag_start, i - tag_start);
                fossil_io_printf("%s", tag);
                fossil_io_cstring_free(tag);
                // Attributes inside tag
                while (i < len && line[i] != '>') {
                    if (line[i] == '=') {
                        fossil_io_printf("{yellow}={normal}");
                        i++;
                    } else if (line[i] == '"' || line[i] == '\'') {
                        char quote = line[i++];
                        fossil_io_printf("{yellow}%c", quote);
                        size_t val_start = i;
                        while (i < len && line[i] != quote) i++;
                        cstring val = fossil_io_cstring_substring(line, val_start, i - val_start);
                        fossil_io_printf("%s", val);
                        fossil_io_cstring_free(val);
                        if (i < len && line[i] == quote) {
                            fossil_io_printf("%c{normal}", quote);
                            i++;
                        }
                    } else {
                        fossil_io_putchar(line[i++]);
                    }
                }
                if (i < len && line[i] == '>') {
                    fossil_io_printf("{yellow,bold}>");
                    i++;
                }
                continue;
            }
            fossil_io_putchar(line[i++]);
        }
        if (line[len-1] != '\n') fossil_io_putchar('\n');
        return;
    }

    // YAML/TOML/INI support
    if (fossil_io_cstring_iequals(ext, "yaml") || fossil_io_cstring_iequals(ext, "yml") ||
        fossil_io_cstring_iequals(ext, "toml") || fossil_io_cstring_iequals(ext, "ini")) {
        while (i < len) {
            // Comments
            if (line[i] == '#') {
                fossil_io_printf("{green}%s{normal}", &line[i]);
                break;
            }
            // Strings
            if (!in_string && (line[i] == '"' || line[i] == '\'')) {
                in_string = true;
                string_quote = line[i];
                fossil_io_printf("{yellow}%c", line[i++]);
                size_t start = i;
                while (i < len && (line[i] != string_quote || (i > start && line[i-1] == '\\'))) i++;
                cstring str = fossil_io_cstring_substring(line, start, i - start);
                fossil_io_printf("%s", str);
                fossil_io_cstring_free(str);
                if (i < len && line[i] == string_quote) {
                    fossil_io_printf("%c{normal}", line[i]);
                    i++;
                }
                in_string = false;
                continue;
            }
            // Key before ':' or '='
            if ((isalpha((unsigned char)line[i]) || line[i] == '_')) {
                size_t key_start = i;
                while (i < len && (isalnum((unsigned char)line[i]) || line[i] == '_' || line[i] == '-')) i++;
                if (i < len && (line[i] == ':' || line[i] == '=')) {
                    cstring key = fossil_io_cstring_substring(line, key_start, i - key_start);
                    fossil_io_printf("{cyan,bold}%s{normal}", key);
                    fossil_io_cstring_free(key);
                    fossil_io_printf("{yellow}%c{normal}", line[i]);
                    i++;
                    continue;
                }
            }
            // Structured keywords
            if (isalpha((unsigned char)line[i])) {
                size_t start = i;
                while (i < len && (isalnum((unsigned char)line[i]) || line[i] == '_')) i++;
                cstring word = fossil_io_cstring_substring(line, start, i - start);
                if (is_structured_keyword(word))
                    fossil_io_printf("{magenta,bold}%s{normal}", word);
                else
                    fossil_io_printf("%s", word);
                fossil_io_cstring_free(word);
                continue;
            }
            // Numbers
            if (isdigit((unsigned char)line[i]) ||
                (line[i] == '-' && i + 1 < len && isdigit((unsigned char)line[i + 1]))) {
                size_t start = i;
                if (line[i] == '-') i++;
                if (line[i] == '0' && i + 1 < len && (line[i+1] == 'x' || line[i+1] == 'X')) {
                    i += 2;
                    while (i < len && isxdigit((unsigned char)line[i])) i++;
                } else {
                    while (i < len && (isdigit((unsigned char)line[i]) || line[i] == '.')) i++;
                }
                cstring num = fossil_io_cstring_substring(line, start, i - start);
                fossil_io_printf("{blue}%s{normal}", num);
                fossil_io_cstring_free(num);
                continue;
            }
            // Punctuation
            if (strchr("{}[]:,=", line[i])) {
                fossil_io_printf("{yellow}%c{normal}", line[i++]);
                continue;
            }
            fossil_io_putchar(line[i++]);
        }
        if (line[len-1] != '\n') fossil_io_putchar('\n');
        return;
    }

    // Plain text fallback
    fossil_io_puts(line);
}

/* -----------------------------------------------------------------------
    Meson build/options file formatting (enhanced keyword highlighter)
    ----------------------------------------------------------------------- */
static const char *KW_MESON[] = {
    "project", "executable", "library", "shared_library", "static_library",
    "dependency", "include_directories", "install", "true", "false", "option",
    "default", "description", "buildtype", "debug", "release", "minimum_version",
    "version", "sources", "files", "subdir", "target", "name", "type", "value",
    "required", "test", "summary", "meson", "foreach", "if", "elif", "else",
    "endif", "assert", "not_found", "get_option", "find_program", "find_library",
    "join_paths", "configure_file", "message", "warning", "error", "run_command",
    "environment", "install_data", "install_headers", "install_subdir", NULL
};

static bool is_meson_keyword(ccstring word) {
    for (int i = 0; KW_MESON[i]; i++) {
        if (fossil_io_cstring_iequals(word, KW_MESON[i]))
            return true;
    }
    return false;
}

static bool is_meson_builtin_func(ccstring word) {
    const char *builtins[] = {
        "get_option", "find_program", "find_library", "join_paths",
        "configure_file", "message", "warning", "error", "run_command",
        "environment", "install_data", "install_headers", "install_subdir", NULL
    };
    for (int i = 0; builtins[i]; i++) {
        if (fossil_io_cstring_iequals(word, builtins[i]))
            return true;
    }
    return false;
}

static void format_meson_line(ccstring line) {
    size_t len = fossil_io_cstring_length(line);
    size_t i = 0;

    // Count leading spaces/tabs for indent
    size_t indent_len = 0;
    while (i < len && (line[i] == ' ' || line[i] == '\t')) {
        indent_len++;
        i++;
    }
    if (indent_len > 0)
        fossil_io_printf("%.*s", (int)indent_len, line);

    while (i < len) {
        // Comments
        if (line[i] == '#') {
            fossil_io_printf("{green}%s{normal}", &line[i]);
            break;
        }

        // Strings (support for triple quotes)
        if ((line[i] == '"' && i + 2 < len && line[i+1] == '"' && line[i+2] == '"') ||
            (line[i] == '\'' && i + 2 < len && line[i+1] == '\'' && line[i+2] == '\'')) {
            char quote = line[i];
            fossil_io_printf("{yellow}%c%c%c", quote, quote, quote);
            i += 3;
            size_t start = i;
            while (i + 2 < len && !(line[i] == quote && line[i+1] == quote && line[i+2] == quote)) i++;
            cstring str = fossil_io_cstring_substring(line, start, i - start);
            fossil_io_printf("%s", str);
            fossil_io_cstring_free(str);
            if (i + 2 < len && line[i] == quote && line[i+1] == quote && line[i+2] == quote) {
                fossil_io_printf("%c%c%c{normal}", quote, quote, quote);
                i += 3;
            }
            continue;
        }
        // Single/double quoted string
        if (line[i] == '"' || line[i] == '\'') {
            char quote = line[i++];
            fossil_io_printf("{yellow}%c", quote);
            size_t start = i;
            while (i < len && line[i] != quote) {
                // Handle escaped quotes
                if (line[i] == '\\' && i + 1 < len && line[i+1] == quote) i++;
                i++;
            }
            cstring str = fossil_io_cstring_substring(line, start, i - start);
            fossil_io_printf("%s", str);
            fossil_io_cstring_free(str);
            if (i < len && line[i] == quote) {
                fossil_io_printf("%c{normal}", quote);
                i++;
            }
            continue;
        }

        // Keywords, builtins, and identifiers
        if (isalpha((unsigned char)line[i]) || line[i] == '_') {
            size_t start = i;
            while (i < len && (isalnum((unsigned char)line[i]) || line[i] == '_')) i++;
            cstring word = fossil_io_cstring_substring(line, start, i - start);
            if (is_meson_keyword(word))
                fossil_io_printf("{cyan,bold}%s{normal}", word);
            else if (is_meson_builtin_func(word))
                fossil_io_printf("{magenta,bold}%s{normal}", word);
            else
                fossil_io_printf("%s", word);
            fossil_io_cstring_free(word);
            continue;
        }

        // Numbers (support for hex, float, negative)
        if (isdigit((unsigned char)line[i]) ||
            (line[i] == '-' && i + 1 < len && isdigit((unsigned char)line[i + 1]))) {
            size_t start = i;
            if (line[i] == '-') i++;
            if (line[i] == '0' && i + 1 < len && (line[i+1] == 'x' || line[i+1] == 'X')) {
                i += 2;
                while (i < len && isxdigit((unsigned char)line[i])) i++;
            } else {
                while (i < len && (isdigit((unsigned char)line[i]) || line[i] == '.')) i++;
            }
            cstring num = fossil_io_cstring_substring(line, start, i - start);
            fossil_io_printf("{blue}%s{normal}", num);
            fossil_io_cstring_free(num);
            continue;
        }

        // Punctuation and operators
        if (strchr("=(),[]{}:+-*/%", line[i])) {
            fossil_io_printf("{blue}%c{normal}", line[i++]);
            continue;
        }

        // Default: print character
        fossil_io_putchar(line[i++]);
    }
}

/* -----------------------------------------------------------------------
    Media: print only metadata
    ----------------------------------------------------------------------- */
static void show_media_info(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        perror("stat");
        return;
    }

    fossil_io_printf("{yellow,bold}Media file:{normal} %s\n", path);
    fossil_io_printf("{green}Size:{normal} %lld bytes\n", (long long)st.st_size);
    fossil_io_printf("{blue}Modification time:{normal} %s", ctime(&st.st_mtime));
}

/* -----------------------------------------------------------------------
    Print timestamps
    ----------------------------------------------------------------------- */
static void show_file_timestamps(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        perror("stat");
        return;
    }
    fossil_io_printf("{magenta,bold}Created:{normal}      %s", ctime(&st.st_ctime));
    fossil_io_printf("{green,bold}Modified:{normal}     %s", ctime(&st.st_mtime));
    fossil_io_printf("{blue,bold}Accessed:{normal}     %s", ctime(&st.st_atime));
}

/* -----------------------------------------------------------------------
    Main function: view file with options
    ----------------------------------------------------------------------- */
int fossil_shark_view(const char *path, bool number_lines,
                             bool number_non_blank, bool squeeze_blank,
                             int head_lines, int tail_lines, bool show_time)
{
     ccstring ext = get_extension(path);

     // If no extension (no dot), treat as plain text
     bool has_extension = strrchr(path, '.') != NULL && ext[0] != '\0';

     /* MEDIA FILE HANDLING */
     if (has_extension && is_media_ext(ext)) {
          show_media_info(path);
          return 0;
     }

     bool binary = is_binary_file(path);

     fossil_io_file_t stream;
     if (fossil_io_file_open(&stream, path, binary ? "rb" : "r") != 0) {
          perror(path);
          return 1;
     }

     if (show_time)
          show_file_timestamps(path);

     /* First: read all lines (needed for head/tail) */
     cstring *lines = NULL;
     size_t count = 0, capacity = 0;

     char buf[2048];
     while (fossil_io_gets_from_stream(buf, sizeof(buf), &stream)) {
         if (count == capacity) {
             size_t new_capacity = capacity ? capacity * 2 : 64;
             if (lines == NULL) {
                 lines = (cstring *)fossil_sys_memory_alloc(new_capacity * sizeof(cstring));
                 if (!lines) {
                     fossil_io_file_close(&stream);
                     fprintf(stderr, "Memory allocation failed\n");
                     return 1;
                 }
             } else {
                 cstring *tmp = (cstring *)fossil_sys_memory_realloc(lines, new_capacity * sizeof(cstring));
                 if (!tmp) {
                     for (size_t j = 0; j < count; j++)
                         fossil_io_cstring_free(lines[j]);
                     fossil_sys_memory_free(lines);
                     fossil_io_file_close(&stream);
                     fprintf(stderr, "Memory allocation failed\n");
                     return 1;
                 }
                 lines = tmp;
             }
             capacity = new_capacity;
         }
         lines[count] = fossil_io_cstring_create(buf);
         if (!lines[count]) {
             for (size_t j = 0; j < count; j++)
                 fossil_io_cstring_free(lines[j]);
             fossil_sys_memory_free(lines);
             fossil_io_file_close(&stream);
             fprintf(stderr, "Memory allocation failed\n");
             return 1;
         }
         count++;
     }
     fossil_io_file_close(&stream);

     int start = 0, end = (int)count;

     if (head_lines > 0 && head_lines < (int)count)
          end = head_lines;

     if (tail_lines > 0 && tail_lines < (int)count)
          start = (int)count - tail_lines;

     bool last_blank = false;
     int printed_ln = 0;

    // Meson file detection (match any file with .meson extension or meson.* in filename)
    bool is_meson_file =
        fossil_io_cstring_iequals(ext, "meson") ||
        fossil_io_cstring_iequals(ext, "wrap") ||
        fossil_io_cstring_iequals(ext, "wrapdb") ||
        fossil_io_cstring_iequals(ext, "wrapfile") ||
        fossil_io_cstring_iequals(ext, "options") ||
        fossil_io_cstring_iequals(ext, "option") ||
        fossil_io_cstring_iequals(ext, "txt") ||
        fossil_io_cstring_contains(path, "meson.build") ||
        fossil_io_cstring_contains(path, "meson_options.txt") ||
        fossil_io_cstring_contains(path, "meson.options") ||
        fossil_io_cstring_contains(path, "meson.");

    for (int i = start; i < end; i++) {
        cstring line = lines[i];
        bool blank = (line[0] == '\n' || line[0] == '\r' || line[0] == '\0');

        if (squeeze_blank && blank) {
             if (last_blank)
                 continue;
             last_blank = true;
        } else {
             last_blank = blank;
        }

        if (number_lines && !(number_non_blank && blank))
             fossil_io_printf("{white,bold}%6d{normal}  ", ++printed_ln);

        // If no extension, treat as plain text (preserve whitespace/newlines)
        if (!has_extension) {
            fossil_io_puts(line);
            continue;
        }

        /* Use all three formatters if all match */
        if (is_structured_ext(ext) && is_code_ext(ext) && is_meson_file) {
             format_structured_line(line, ext);
             format_code_line(line);
             format_meson_line(line);
             if (line[fossil_io_cstring_length(line)-1] != '\n')
                 fossil_io_putchar('\n');
             continue;
        }

        /* Use both structured and code formatting if both match */
        if (is_structured_ext(ext) && is_code_ext(ext)) {
             format_structured_line(line, ext);
             format_code_line(line);
             if (line[fossil_io_cstring_length(line)-1] != '\n')
                 fossil_io_putchar('\n');
             continue;
        }

        /* Use both structured and meson formatting if both match */
        if (is_structured_ext(ext) && is_meson_file) {
             format_structured_line(line, ext);
             format_meson_line(line);
             if (line[fossil_io_cstring_length(line)-1] != '\n')
                 fossil_io_putchar('\n');
             continue;
        }

        /* Meson formatting */
        if (is_meson_file) {
             format_meson_line(line);
             if (line[fossil_io_cstring_length(line)-1] != '\n')
                 fossil_io_putchar('\n');
             continue;
        }

        /* Structured data pretty-print */
        if (is_structured_ext(ext)) {
             format_structured_line(line, ext);
             if (line[fossil_io_cstring_length(line)-1] != '\n')
                 fossil_io_putchar('\n');
             continue;
        }

        /* Code formatting */
        if (is_code_ext(ext)) {
             format_code_line(line);
             if (line[fossil_io_cstring_length(line)-1] != '\n')
                 fossil_io_putchar('\n');
             continue;
        }

        /* Default: just print the line (preserve whitespace/newlines) */
        fossil_io_puts(line);
    }

     if (lines) {
         for (size_t i = 0; i < count; i++)
             fossil_io_cstring_free(lines[i]);
         fossil_sys_memory_free(lines);
     }

     return 0;
}
