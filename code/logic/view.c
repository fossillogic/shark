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

// =========================================================
// Meson build files
// =========================================================

typedef struct {
    const char* extension;
    const char* name;
} fossil_meson_format_t;

static const fossil_meson_format_t meson_formats[] = {
    {"meson.options", "Meson Build Options"},
    {"meson.build", "Meson Build Script"},
    {".wrap", "Meson Wrap File"},
    {NULL, NULL}
};

static int fossil_meson_lookup(const char* filename) {
    if (!filename) return -1;
    for (int i = 0; meson_formats[i].extension != NULL; i++) {
        if (strstr(filename, meson_formats[i].extension) != NULL) {
            return i;
        }
    }
    return -1;
}

static bool fossil_meson_is_indent_keyword(const char* line) {
    const char* indent_keywords[] = {
        "if", "foreach", "func", "while", NULL
    };
    for (int i = 0; indent_keywords[i] != NULL; i++) {
        if (strstr(line, indent_keywords[i]) != NULL) return true;
    }
    return false;
}

static bool fossil_meson_is_dedent_keyword(const char* line) {
    const char* dedent_keywords[] = {
        "endif", "endforeach", "endwhile", "endfunc", "else", "elif", NULL
    };
    for (int i = 0; dedent_keywords[i] != NULL; i++) {
        if (strstr(line, dedent_keywords[i]) != NULL) return true;
    }
    return false;
}

static void fossil_meson_colorize_keyword(const char* keyword) {
    fputs("\033[35m", stdout);
    fputs(keyword, stdout);
    fputs("\033[0m", stdout);
}

static void fossil_meson_colorize_string(const char* str) {
    fputs("\033[32m", stdout);
    fputs(str, stdout);
    fputs("\033[0m", stdout);
}

static void fossil_meson_format_line(const char* line, int indent_level) {
    if (!line) return;
    
    while (*line && isspace(*line)) line++;
    
    for (int i = 0; i < indent_level * 2; i++) fputc(' ', stdout);
    
    bool in_string = false;
    char quote_char = 0;
    
    for (const char* p = line; *p; p++) {
        if ((*p == '"' || *p == '\'') && (p == line || *(p-1) != '\\')) {
            if (!in_string) {
                in_string = true;
                quote_char = *p;
                fossil_meson_colorize_string("'");
            } else if (*p == quote_char) {
                in_string = false;
                fossil_meson_colorize_string("'");
            } else {
                fputc(*p, stdout);
            }
        } else if (!in_string && isalpha(*p)) {
            const char* start = p;
            while (*p && (isalnum(*p) || *p == '_')) p++;
            char temp[256];
            memcpy(temp, start, p - start);
            temp[p - start] = '\0';
            fossil_meson_colorize_keyword(temp);
            p--;
        } else {
            fputc(*p, stdout);
        }
    }
}

static int fossil_meson_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    int indent = 0;
    char buffer[512];
    const char* ptr = content;
    
    while (*ptr) {
        const char* line_end = strchr(ptr, '\n');
        if (!line_end) line_end = ptr + strlen(ptr);
        
        size_t line_len = line_end - ptr;
        if (line_len >= sizeof(buffer)) line_len = sizeof(buffer) - 1;
        memcpy(buffer, ptr, line_len);
        buffer[line_len] = '\0';
        
        char* trimmed = buffer;
        while (*trimmed && isspace(*trimmed)) trimmed++;
        
        if (!*trimmed) {
            fputc('\n', stdout);
            ptr = line_end;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        if (smart_indent) {
            if (fossil_meson_is_dedent_keyword(trimmed)) {
                indent = (indent > 0) ? indent - 1 : 0;
            }
            fossil_meson_format_line(trimmed, indent);
            if (fossil_meson_is_indent_keyword(trimmed)) {
                indent++;
            }
        } else {
            fputs(trimmed, stdout);
        }
        
        fputc('\n', stdout);
        ptr = line_end;
        if (*ptr == '\n') ptr++;
    }
    return 0;
}

// =========================================================
// Source code files (C, C++, etc.)
// =========================================================

typedef struct {
    const char* extension;
    const char* name;
} fossil_source_format_t;

static const fossil_source_format_t source_formats[] = {
    {".c", "C Source"},
    {".h", "C Header"},
    {".cpp", "C++ Source"},
    {".cc", "C++ Source"},
    {".cxx", "C++ Source"},
    {".hpp", "C++ Header"},
    {".hh", "C++ Header"},
    {".hxx", "C++ Header"},
    {NULL, NULL}
};

static int fossil_source_lookup(const char* filename) {
    if (!filename) return -1;
    const char* ext = strrchr(filename, '.');
    if (!ext) return -1;
    for (int i = 0; source_formats[i].extension != NULL; i++) {
        if (strcmp(ext, source_formats[i].extension) == 0) {
            return i;
        }
    }
    return -1;
}

static bool fossil_source_is_block_start(const char* line) {
    const char* keywords[] = {"{", "if(", "if (", "for(", "for (", "while(", "while (", NULL};
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strstr(line, keywords[i]) != NULL) return true;
    }
    return false;
}

static bool fossil_source_is_block_end(const char* line) {
    return strchr(line, '}') != NULL;
}

static void fossil_source_colorize_comment(const char* comment) {
    fputs("\033[90m", stdout);
    fputs(comment, stdout);
    fputs("\033[0m", stdout);
}

static int fossil_source_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    int indent = 0;
    const char* ptr = content;
    bool in_multiline_comment = false;
    
    while (*ptr) {
        const char* line_end = strchr(ptr, '\n');
        if (!line_end) line_end = ptr + strlen(ptr);
        
        size_t line_len = line_end - ptr;
        char buffer[2048];
        if (line_len >= sizeof(buffer)) line_len = sizeof(buffer) - 1;
        memcpy(buffer, ptr, line_len);
        buffer[line_len] = '\0';
        
        char* trimmed = buffer;
        int leading_spaces = 0;
        
        while (*trimmed && isspace(*trimmed)) {
            trimmed++;
            leading_spaces++;
        }
        
        if (!*trimmed) {
            fputc('\n', stdout);
            ptr = line_end;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        // Handle multiline comments
        if (strstr(trimmed, "/*") != NULL) in_multiline_comment = true;
        
        if (in_multiline_comment) {
            fossil_source_colorize_comment(trimmed);
            if (strstr(trimmed, "*/") != NULL) in_multiline_comment = false;
        } else if (smart_indent) {
            if (fossil_source_is_block_end(trimmed)) {
                indent = (indent > 0) ? indent - 1 : 0;
            }
            
            for (int i = 0; i < indent * 4; i++) fputc(' ', stdout);
            fputs(trimmed, stdout);
            
            if (fossil_source_is_block_start(trimmed)) {
                indent++;
            }
        } else {
            for (int i = 0; i < leading_spaces; i++) fputc(' ', stdout);
            fputs(trimmed, stdout);
        }
        
        fputc('\n', stdout);
        ptr = line_end;
        if (*ptr == '\n') ptr++;
    }
    return 0;
}

// =========================================================
// Structured data files (e.g., JSON, XML, YAML)
// =========================================================

typedef struct {
    const char* extension;
    const char* name;
} fossil_data_format_t;

static const fossil_data_format_t data_formats[] = {
    {".json", "JSON"},
    {".fson", "FSON"},
    {".xml", "XML"},
    {".yaml", "YAML"},
    {".yml", "YAML"},
    {".toml", "TOML"},
    {".ini", "INI"},
    {".cfg", "Configuration"},
    {".conf", "Configuration"},
    {".csv", "CSV"},
    {".tsv", "TSV"},
    {NULL, NULL}
};

static int fossil_data_lookup(const char* filename) {
    if (!filename) return -1;
    const char* ext = strrchr(filename, '.');
    if (!ext) return -1;
    for (int i = 0; data_formats[i].extension != NULL; i++) {
        if (strcmp(ext, data_formats[i].extension) == 0) {
            return i;
        }
    }
    return -1;
}

static int fossil_json_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    int indent = 0;
    const char* ptr = content;
    bool in_string = false;
    
    while (*ptr) {
        const char* line_end = strchr(ptr, '\n');
        if (!line_end) line_end = ptr + strlen(ptr);
        
        size_t line_len = line_end - ptr;
        char buffer[2048];
        if (line_len >= sizeof(buffer)) line_len = sizeof(buffer) - 1;
        memcpy(buffer, ptr, line_len);
        buffer[line_len] = '\0';
        
        char* trimmed = buffer;
        while (*trimmed && isspace(*trimmed)) trimmed++;
        
        if (!*trimmed) {
            fputc('\n', stdout);
            ptr = line_end;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        if (smart_indent && (*trimmed == '}' || *trimmed == ']')) {
            indent = (indent > 0) ? indent - 1 : 0;
        }
        
        if (smart_indent) {
            for (int i = 0; i < indent * 2; i++) fputc(' ', stdout);
        }
        
        for (const char* p = trimmed; *p; p++) {
            if (*p == '"' && (p == trimmed || *(p-1) != '\\')) {
                in_string = !in_string;
                fputs(in_string ? "\033[32m" : "\033[0m", stdout);
            }
            fputc(*p, stdout);
        }
        fputc('\n', stdout);
        
        if (smart_indent) {
            in_string = false;
            for (const char* p = trimmed; *p; p++) {
                if (*p == '"' && (p == trimmed || *(p-1) != '\\')) in_string = !in_string;
                if (!in_string && (*p == '{' || *p == '[')) indent++;
            }
        }
        
        ptr = line_end;
        if (*ptr == '\n') ptr++;
    }
    return 0;
}

static int fossil_xml_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    int indent = 0;
    const char* ptr = content;
    bool in_comment = false;
    
    while (*ptr) {
        const char* line_end = strchr(ptr, '\n');
        if (!line_end) line_end = ptr + strlen(ptr);
        
        size_t line_len = line_end - ptr;
        char buffer[2048];
        if (line_len >= sizeof(buffer)) line_len = sizeof(buffer) - 1;
        memcpy(buffer, ptr, line_len);
        buffer[line_len] = '\0';
        
        char* trimmed = buffer;
        while (*trimmed && isspace(*trimmed)) trimmed++;
        
        if (!*trimmed) {
            fputc('\n', stdout);
            ptr = line_end;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        if (strstr(trimmed, "<!--") != NULL) in_comment = true;
        
        if (smart_indent && (strstr(trimmed, "</") == trimmed || *trimmed == '}') && !in_comment) {
            indent = (indent > 0) ? indent - 1 : 0;
        }
        
        if (smart_indent) {
            for (int i = 0; i < indent * 2; i++) fputc(' ', stdout);
        }
        
        if (in_comment) fputs("\033[90m", stdout);
        else if (strstr(trimmed, "<?") == trimmed) fputs("\033[36m", stdout);
        else fputs("\033[0m", stdout);
        
        fputs(trimmed, stdout);
        
        if (in_comment || strstr(trimmed, "<?") == trimmed) fputs("\033[0m", stdout);
        fputc('\n', stdout);
        
        if (strstr(trimmed, "-->") != NULL) in_comment = false;
        
        if (smart_indent && strstr(trimmed, "<") && !strstr(trimmed, "</") && 
            !strstr(trimmed, "/>") && !in_comment) {
            indent++;
        }
        
        ptr = line_end;
        if (*ptr == '\n') ptr++;
    }
    return 0;
}

static int fossil_yaml_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    const char* ptr = content;
    int base_indent = -1;
    
    while (*ptr) {
        const char* line_end = strchr(ptr, '\n');
        if (!line_end) line_end = ptr + strlen(ptr);
        
        size_t line_len = line_end - ptr;
        char buffer[2048];
        if (line_len >= sizeof(buffer)) line_len = sizeof(buffer) - 1;
        memcpy(buffer, ptr, line_len);
        buffer[line_len] = '\0';
        
        char* trimmed = buffer;
        int leading = 0;
        while (*trimmed && isspace(*trimmed)) {
            trimmed++;
            leading++;
        }
        
        if (!*trimmed) {
            fputc('\n', stdout);
        } else {
            if (base_indent == -1 && leading > 0) base_indent = leading;
            
            if (smart_indent) {
                for (int i = 0; i < leading; i++) fputc(' ', stdout);
            }
            
            for (const char* p = trimmed; *p; p++) {
                if (*p == ':' && *(p+1) == ' ') {
                    fputs("\033[33m", stdout);
                    fputc(*p, stdout);
                    fputs("\033[0m", stdout);
                } else if (*p == '#') {
                    fputs("\033[90m", stdout);
                    while (*p) fputc(*(p++), stdout);
                    fputs("\033[0m", stdout);
                    break;
                } else {
                    fputc(*p, stdout);
                }
            }
            fputc('\n', stdout);
        }
        
        ptr = line_end;
        if (*ptr == '\n') ptr++;
    }
    return 0;
}

static int fossil_toml_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    const char* ptr = content;
    bool in_string = false;
    
    while (*ptr) {
        const char* line_end = strchr(ptr, '\n');
        if (!line_end) line_end = ptr + strlen(ptr);
        
        size_t line_len = line_end - ptr;
        char buffer[2048];
        if (line_len >= sizeof(buffer)) line_len = sizeof(buffer) - 1;
        memcpy(buffer, ptr, line_len);
        buffer[line_len] = '\0';
        
        char* trimmed = buffer;
        while (*trimmed && isspace(*trimmed)) trimmed++;
        
        if (!*trimmed || *trimmed == '#') {
            fputs("\033[90m", stdout);
            fputs(buffer, stdout);
            fputs("\033[0m\n", stdout);
        } else if (*trimmed == '[') {
            fputs("\033[1;36m", stdout);
            fputs(trimmed, stdout);
            fputs("\033[0m\n", stdout);
        } else {
            for (const char* p = trimmed; *p; p++) {
                if (*p == '"') {
                    in_string = !in_string;
                    fputs(in_string ? "\033[32m" : "\033[0m", stdout);
                    fputc(*p, stdout);
                } else if (*p == '=' && !in_string) {
                    fputs("\033[0m", stdout);
                    fputc(*p, stdout);
                } else {
                    fputc(*p, stdout);
                }
            }
            fputs("\033[0m\n", stdout);
        }
        
        ptr = line_end;
        if (*ptr == '\n') ptr++;
    }
    return 0;
}

static int fossil_ini_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    char buffer[2048];
    const char* ptr = content;
    
    while (sscanf(ptr, "%2047[^\n]", buffer) > 0) {
        char* trimmed = buffer;
        while (*trimmed && isspace(*trimmed)) trimmed++;
        
        if (!*trimmed) {
            fputc('\n', stdout);
        } else if (*trimmed == ';' || *trimmed == '#') {
            fputs("\033[90m", stdout);
            fputs(trimmed, stdout);
            fputs("\033[0m\n", stdout);
        } else if (*trimmed == '[' && trimmed[strlen(trimmed)-1] == ']') {
            fputs("\033[1;33m", stdout);
            fputs(trimmed, stdout);
            fputs("\033[0m\n", stdout);
        } else {
            char* eq = strchr(trimmed, '=');
            if (eq) {
                *eq = '\0';
                fputs(trimmed, stdout);
                fputs("\033[0m=", stdout);
                fputs(eq + 1, stdout);
            } else {
                fputs(trimmed, stdout);
            }
            fputs("\033[0m\n", stdout);
        }
        
        ptr = strchr(ptr, '\n');
        if (!ptr) break;
        ptr++;
    }
    return 0;
}

static int fossil_csv_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    char buffer[4096];
    const char* ptr = content;
    bool in_quotes = false;
    
    while (sscanf(ptr, "%4095[^\n]", buffer) > 0) {
        for (const char* p = buffer; *p; p++) {
            if (*p == '"') {
                in_quotes = !in_quotes;
                fputs("\033[32m", stdout);
                fputc(*p, stdout);
                fputs("\033[0m", stdout);
            } else if (*p == ',' && !in_quotes) {
                fputs("\033[33m", stdout);
                fputc(*p, stdout);
                fputs("\033[0m", stdout);
            } else {
                fputc(*p, stdout);
            }
        }
        fputc('\n', stdout);
        
        ptr = strchr(ptr, '\n');
        if (!ptr) break;
        ptr++;
    }
    return 0;
}

static int fossil_tsv_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    char buffer[4096];
    const char* ptr = content;
    
    while (sscanf(ptr, "%4095[^\n]", buffer) > 0) {
        for (const char* p = buffer; *p; p++) {
            if (*p == '\t') {
                fputs("\033[33m", stdout);
                fputc(*p, stdout);
                fputs("\033[0m", stdout);
            } else {
                fputc(*p, stdout);
            }
        }
        fputc('\n', stdout);
        
        ptr = strchr(ptr, '\n');
        if (!ptr) break;
        ptr++;
    }
    return 0;
}

static int fossil_fson_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
        
    int indent = 0;
    const char* ptr = content;
    bool in_string = false;
    
    while (*ptr) {
        const char* line_end = strchr(ptr, '\n');
        if (!line_end) line_end = ptr + strlen(ptr);
        
        size_t line_len = line_end - ptr;
        char buffer[2048];
        if (line_len >= sizeof(buffer)) line_len = sizeof(buffer) - 1;
        memcpy(buffer, ptr, line_len);
        buffer[line_len] = '\0';
        
        char* trimmed = buffer;
        while (*trimmed && isspace(*trimmed)) trimmed++;
        
        if (!*trimmed) {
            fputc('\n', stdout);
            ptr = line_end;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        if (smart_indent && (*trimmed == '}' || *trimmed == ']')) {
            indent = (indent > 0) ? indent - 1 : 0;
        }
        
        if (smart_indent) {
            for (int i = 0; i < indent * 2; i++) fputc(' ', stdout);
        }
        
        for (const char* p = trimmed; *p; p++) {
            if ((p == trimmed || isspace(*(p-1)) || *p == ':') && 
                (strstr(p, "cstr:") == p || strstr(p, "i8:") == p ||
                 strstr(p, "i16:") == p || strstr(p, "i32:") == p || 
                 strstr(p, "i64:") == p || strstr(p, "u8:") == p ||
                 strstr(p, "u16:") == p || strstr(p, "u32:") == p ||
                 strstr(p, "u64:") == p || strstr(p, "f32:") == p ||
                 strstr(p, "f64:") == p || strstr(p, "bool:") == p ||
                 strstr(p, "character:") == p || strstr(p, "enum_val:") == p ||
                 strstr(p, "datetime:") == p || strstr(p, "duration:") == p ||
                 strstr(p, "object:") == p || strstr(p, "array:") == p ||
                 strstr(p, "null:") == p || strstr(p, "oct:") == p ||
                 strstr(p, "hex:") == p || strstr(p, "bin:") == p)) {
                fputs("\033[35m", stdout);
                while (*p && *p != ':') fputc(*(p++), stdout);
                fputc(':', stdout);
                fputs("\033[0m", stdout);
                continue;
            }
            
            if (*p == '"' && (p == trimmed || *(p-1) != '\\')) {
                in_string = !in_string;
                fputs(in_string ? "\033[32m" : "\033[0m", stdout);
            }
            fputc(*p, stdout);
        }
        fputc('\n', stdout);
        
        if (smart_indent) {
            in_string = false;
            for (const char* p = trimmed; *p; p++) {
                if (*p == '"' && (p == trimmed || *(p-1) != '\\')) in_string = !in_string;
                if (!in_string && (*p == '{' || *p == '[')) indent++;
            }
        }
        
        ptr = line_end;
        if (*ptr == '\n') ptr++;
    }
    return 0;
}

static int fossil_data_backend_format(const char* content, const char* filename, bool smart_indent) {
    if (!content || !filename) return -1;
    
    int format_id = fossil_data_lookup(filename);
    if (format_id == -1) return -1;
    
    switch (format_id) {
        case 0: return fossil_json_backend_format(content, smart_indent);
        case 1: return fossil_fson_backend_format(content, smart_indent);
        case 2: return fossil_xml_backend_format(content, smart_indent);
        case 3:
        case 4: return fossil_yaml_backend_format(content, smart_indent);
        case 5: return fossil_toml_backend_format(content, smart_indent);
        case 6: return fossil_ini_backend_format(content, smart_indent);
        case 7:
        case 8: return fossil_csv_backend_format(content, smart_indent);
        case 9: return fossil_csv_backend_format(content, smart_indent);
        case 10: return fossil_tsv_backend_format(content, smart_indent);
        default: return -1;
    }
}

// =========================================================
// Documentation files (e.g., Markdown, reStructuredText)
// =========================================================

typedef struct {
    const char* extension;
    const char* name;
} fossil_doc_format_t;

static const fossil_doc_format_t doc_formats[] = {
    {".md", "Markdown"},
    {".markdown", "Markdown"},
    {".rst", "reStructuredText"},
    {".txt", "Plain Text"},
    {".asciidoc", "AsciiDoc"},
    {".adoc", "AsciiDoc"},
    {NULL, NULL}
};

static int fossil_doc_lookup(const char* filename) {
    if (!filename) return -1;
    const char* ext = strrchr(filename, '.');
    if (!ext) return -1;
    for (int i = 0; doc_formats[i].extension != NULL; i++) {
        if (strcmp(ext, doc_formats[i].extension) == 0) {
            return i;
        }
    }
    return -1;
}

static void fossil_markdown_apply_emphasis(const char* line) {
    bool bold = false, italic = false, code = false;
    for (const char* p = line; *p; p++) {
        if (*p == '*' && *(p+1) == '*') {
            bold = !bold;
            fputs(bold ? "\033[1m" : "\033[0m", stdout);
            p++;
            continue;
        }
        if (*p == '*' || *p == '_') {
            italic = !italic;
            fputs(italic ? "\033[3m" : "\033[0m", stdout);
            continue;
        }
        if (*p == '`') {
            code = !code;
            fputs(code ? "\033[36m" : "\033[0m", stdout);
            continue;
        }
        fputc(*p, stdout);
    }
    fputs("\033[0m", stdout);
}

static int fossil_markdown_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    int list_indent = 0, heading_level = 0;
    char buffer[2048];
    const char* ptr = content;
    bool in_code_block = false;
    
    while (sscanf(ptr, "%2047[^\n]", buffer) > 0) {
        char* trimmed = buffer;
        while (*trimmed && isspace(*trimmed)) trimmed++;
        
        if (!*trimmed) {
            fputc('\n', stdout);
            ptr = strchr(ptr, '\n');
            if (!ptr) break;
            ptr++;
            continue;
        }
        
        // Handle code blocks
        if (strstr(trimmed, "```") != NULL) {
            in_code_block = !in_code_block;
            fputs(trimmed, stdout);
            fputc('\n', stdout);
            ptr = strchr(ptr, '\n');
            if (!ptr) break;
            ptr++;
            continue;
        }
        
        // Handle headings
        if (*trimmed == '#') {
            heading_level = 0;
            while (heading_level < 6 && *(trimmed + heading_level) == '#') heading_level++;
            fputs("\033[1m", stdout);
            fputs(trimmed, stdout);
            fputs("\033[0m\n", stdout);
            ptr = strchr(ptr, '\n');
            if (!ptr) break;
            ptr++;
            continue;
        }
        
        // Handle lists
        if (smart_indent) {
            if (*trimmed == '-' || *trimmed == '*' || *trimmed == '+' || 
                (isdigit(*trimmed) && strchr(trimmed, '.') && *(strchr(trimmed, '.') + 1) == ' ')) {
                if (list_indent == 0) list_indent = 1;
            } else if (list_indent > 0 && *trimmed != '-' && *trimmed != '*' && *trimmed != '+') {
                list_indent = 0;
            }
            
            if (list_indent > 0) {
                for (int i = 0; i < list_indent * 2; i++) fputc(' ', stdout);
            }
        }
        
        // Apply formatting if not in code block
        if (in_code_block) {
            fputs(trimmed, stdout);
        } else {
            fossil_markdown_apply_emphasis(trimmed);
        }
        fputc('\n', stdout);
        
        ptr = strchr(ptr, '\n');
        if (!ptr) break;
        ptr++;
    }
    return 0;
}

static int fossil_rst_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    char buffer[2048];
    const char* ptr = content;
    bool in_code_block = false;
    int indent_level = 0;
    
    while (sscanf(ptr, "%2047[^\n]", buffer) > 0) {
        char* trimmed = buffer;
        int leading_spaces = 0;
        while (*trimmed && isspace(*trimmed)) {
            trimmed++;
            leading_spaces++;
        }
        
        // Detect code blocks
        if (strstr(trimmed, "::") != NULL) {
            in_code_block = true;
            indent_level = leading_spaces;
            fputs(buffer, stdout);
            fputc('\n', stdout);
            ptr = strchr(ptr, '\n');
            if (!ptr) break;
            ptr++;
            continue;
        }
        
        // Detect code block end
        if (in_code_block && leading_spaces <= indent_level && *trimmed != '\0') {
            in_code_block = false;
        }
        
        // Apply formatting
        if (in_code_block) {
            fputs("\033[36m", stdout);
        }
        
        fputs(buffer, stdout);
        if (in_code_block) fputs("\033[0m", stdout);
        fputc('\n', stdout);
        
        ptr = strchr(ptr, '\n');
        if (!ptr) break;
        ptr++;
    }
    return 0;
}

static int fossil_asciidoc_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    char buffer[2048];
    const char* ptr = content;
    bool in_code = false;
    
    while (sscanf(ptr, "%2047[^\n]", buffer) > 0) {
        char* trimmed = buffer;
        while (*trimmed && isspace(*trimmed)) trimmed++;
        
        // Handle code blocks
        if (strstr(trimmed, "----") != NULL || strstr(trimmed, "....") != NULL) {
            in_code = !in_code;
        }
        
        if (in_code) {
            fputs("\033[36m", stdout);
        } else if (*trimmed == '=' && *(trimmed + 1) == ' ') {
            fputs("\033[1m", stdout);
        }
        
        fputs(buffer, stdout);
        
        if (in_code || (*trimmed == '=' && *(trimmed + 1) == ' ')) {
            fputs("\033[0m", stdout);
        }
        fputc('\n', stdout);
        
        ptr = strchr(ptr, '\n');
        if (!ptr) break;
        ptr++;
    }
    return 0;
}

static int fossil_plaintext_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    fputs(content, stdout);
    return 0;
}

static int fossil_doc_backend_format(const char* content, const char* filename, bool smart_indent) {
    if (!content || !filename) return -1;
    
    int format_id = fossil_doc_lookup(filename);
    if (format_id == -1) return fossil_plaintext_backend_format(content, smart_indent);
    
    switch (format_id) {
        case 0:
        case 1: return fossil_markdown_backend_format(content, smart_indent);
        case 2: return fossil_rst_backend_format(content, smart_indent);
        case 3: return fossil_plaintext_backend_format(content, smart_indent);
        case 4:
        case 5: return fossil_asciidoc_backend_format(content, smart_indent);
        default: return fossil_plaintext_backend_format(content, smart_indent);
    }
}

int fossil_shark_view(ccstring path, bool number_lines,
                      bool number_non_blank, bool squeeze_blank, bool format,
                      int head_lines, int tail_lines, bool show_time) {
    if (!path) return -1;
    
    FILE* file = fopen(path, "r");
    if (!file) return -1;
    
    // Read entire file content
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = malloc(file_size + 1);
    if (!content) {
        fclose(file);
        return -1;
    }
    
    size_t read_size = fread(content, 1, file_size, file);
    content[read_size] = '\0';
    fclose(file);
    
    // Apply formatting if requested
    if (format) {
        if (fossil_meson_lookup(path) != -1) {
            fossil_meson_backend_format(content, true);
        } else if (fossil_source_lookup(path) != -1) {
            fossil_source_backend_format(content, true);
        } else if (fossil_data_lookup(path) != -1) {
            fossil_data_backend_format(content, path, true);
        } else if (fossil_doc_lookup(path) != -1) {
            fossil_doc_backend_format(content, path, true);
        } else {
            fputs(content, stdout);
        }
    } else {
        fputs(content, stdout);
    }
    
    free(content);
    return 0;
}
