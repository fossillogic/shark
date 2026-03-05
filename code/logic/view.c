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

static int fossil_doc_lookup(const char* filename);

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
    fossil_io_puts("\033[35m");
    fossil_io_puts(keyword);
    fossil_io_puts("\033[0m");
}

static void fossil_meson_colorize_string(const char* str) {
    fossil_io_puts("\033[32m");
    fossil_io_puts(str);
    fossil_io_puts("\033[0m");
}

static void fossil_meson_format_line(const char* line, int indent_level) {
    if (!line) return;
    
    while (*line && isspace(*line)) line++;
    
    for (int i = 0; i < indent_level * 2; i++) fossil_io_putchar(' ');
    
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
                fossil_io_putchar(*p);
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
            fossil_io_putchar(*p);
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
            fossil_io_putchar('\n');
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
            fossil_io_puts(trimmed);
        }
        
        fossil_io_putchar('\n');
        ptr = line_end;
        if (*ptr == '\n') ptr++;
    }
    return 0;
}

int fossil_shark_view(ccstring path, bool format) {
    if (!path) return -1;
    
    fossil_io_file_t stream = {0};
    if (fossil_io_file_open(&stream, path, "r") != 0) {
        return -1;
    }
    
    // Read entire file content
    fossil_io_file_seek(&stream, 0, SEEK_END);
    size_t file_size = fossil_io_file_get_size(&stream);
    fossil_io_file_seek(&stream, 0, SEEK_SET);
    
    cstring content = fossil_io_cstring_create("");
    if (!content) {
        fossil_io_file_close(&stream);
        return -1;
    }
    
    char buffer[4096];
    size_t read_size;
    while ((read_size = fossil_io_file_read(&stream, buffer, 1, sizeof(buffer) - 1)) > 0) {
        buffer[read_size] = '\0';
        content = fossil_io_cstring_append(&content, buffer);
        if (!content) {
            fossil_io_file_close(&stream);
            return -1;
        }
    }
    
    fossil_io_file_close(&stream);
    
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
            fossil_io_puts(content);
        }
    } else {
        fossil_io_puts(content);
    }
    
    fossil_io_cstring_free(content);
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
    fossil_io_puts("\033[90m");
    fossil_io_puts(comment);
    fossil_io_puts("\033[0m");
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
            fossil_io_putchar('\n');
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
            
            for (int i = 0; i < indent * 4; i++) fossil_io_putchar(' ');
            fossil_io_puts(trimmed);
            
            if (fossil_source_is_block_start(trimmed)) {
                indent++;
            }
        } else {
            for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
            fossil_io_puts(trimmed);
        }
        
        fossil_io_putchar('\n');
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
            fossil_io_putchar('\n');
            ptr = line_end;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        if (smart_indent && (*trimmed == '}' || *trimmed == ']')) {
            indent = (indent > 0) ? indent - 1 : 0;
        }
        
        if (smart_indent) {
            for (int i = 0; i < indent * 2; i++) fossil_io_putchar(' ');
        }
        
        for (const char* p = trimmed; *p; p++) {
            if (*p == '"' && (p == trimmed || *(p-1) != '\\')) {
                in_string = !in_string;
                fossil_io_puts(in_string ? "\033[32m" : "\033[0m");
            }
            fossil_io_putchar(*p);
        }
        fossil_io_putchar('\n');
        
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
            fossil_io_putchar('\n');
            ptr = line_end;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        if (strstr(trimmed, "<!--") != NULL) in_comment = true;
        
        if (smart_indent && (strstr(trimmed, "</") == trimmed || *trimmed == '}') && !in_comment) {
            indent = (indent > 0) ? indent - 1 : 0;
        }
        
        if (smart_indent) {
            for (int i = 0; i < indent * 2; i++) fossil_io_putchar(' ');
        }
        
        if (in_comment) fossil_io_puts("\033[90m");
        else if (strstr(trimmed, "<?") == trimmed) fossil_io_puts("\033[36m");
        else fossil_io_puts("\033[0m");
        
        fossil_io_puts(trimmed);
        
        if (in_comment || strstr(trimmed, "<?") == trimmed) fossil_io_puts("\033[0m");
        fossil_io_putchar('\n');
        
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
            fossil_io_putchar('\n');
        } else {
            if (smart_indent) {
                if (base_indent == -1 && leading > 0) base_indent = leading;
                int indent_level = (leading - base_indent) / 2;
                if (indent_level < 0) indent_level = 0;
                for (int i = 0; i < indent_level * 2; i++) fossil_io_putchar(' ');
            } else {
                for (int i = 0; i < leading; i++) fossil_io_putchar(' ');
            }
            
            for (const char* p = trimmed; *p; p++) {
                if (*p == ':' && *(p+1) == ' ') {
                    fossil_io_puts("\033[33m");
                    fossil_io_putchar(*p);
                    fossil_io_puts("\033[0m");
                } else if (*p == '#') {
                    fossil_io_puts("\033[90m");
                    while (*p) fossil_io_putchar(*(p++));
                    fossil_io_puts("\033[0m");
                    break;
                } else {
                    fossil_io_putchar(*p);
                }
            }
            fossil_io_putchar('\n');
        }
        
        ptr = line_end;
        if (*ptr == '\n') ptr++;
    }
    return 0;
}

static int fossil_toml_backend_format(const char* content, bool smart_indent) {
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
        
        if (smart_indent && (*trimmed == ']')) {
            indent = (indent > 0) ? indent - 1 : 0;
        }
        
        if (smart_indent) {
            for (int i = 0; i < indent * 2; i++) fossil_io_putchar(' ');
        }
        
        if (!*trimmed || *trimmed == '#') {
            fossil_io_puts("\033[90m");
            fossil_io_puts(trimmed);
            fossil_io_puts("\033[0m\n");
        } else if (*trimmed == '[') {
            fossil_io_puts("\033[1;36m");
            fossil_io_puts(trimmed);
            fossil_io_puts("\033[0m\n");
        } else {
            for (const char* p = trimmed; *p; p++) {
                if (*p == '"') {
                    in_string = !in_string;
                    fossil_io_puts(in_string ? "\033[32m" : "\033[0m");
                    fossil_io_putchar(*p);
                } else if (*p == '=' && !in_string) {
                    fossil_io_puts("\033[0m");
                    fossil_io_putchar(*p);
                } else {
                    fossil_io_putchar(*p);
                }
            }
            fossil_io_puts("\033[0m\n");
        }
        
        if (smart_indent) {
            in_string = false;
            for (const char* p = trimmed; *p; p++) {
                if (*p == '"') in_string = !in_string;
                if (!in_string && *p == '[') indent++;
            }
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
    int leading_spaces = 0;
    
    while (sscanf(ptr, "%2047[^\n]", buffer) > 0) {
        char* trimmed = buffer;
        leading_spaces = 0;
        while (*trimmed && isspace(*trimmed)) {
            trimmed++;
            leading_spaces++;
        }
        
        if (!*trimmed) {
            fossil_io_putchar('\n');
        } else if (*trimmed == ';' || *trimmed == '#') {
            if (smart_indent) {
                for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
            }
            fossil_io_puts("\033[90m");
            fossil_io_puts(trimmed);
            fossil_io_puts("\033[0m\n");
        } else if (*trimmed == '[' && trimmed[strlen(trimmed)-1] == ']') {
            if (smart_indent) {
                for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
            }
            fossil_io_puts("\033[1;33m");
            fossil_io_puts(trimmed);
            fossil_io_puts("\033[0m\n");
        } else {
            if (smart_indent) {
                for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
            }
            char* eq = strchr(trimmed, '=');
            if (eq) {
                *eq = '\0';
                fossil_io_puts(trimmed);
                fossil_io_puts("\033[0m=");
                fossil_io_puts(eq + 1);
            } else {
                fossil_io_puts(trimmed);
            }
            fossil_io_puts("\033[0m\n");
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
        char* trimmed = buffer;
        int leading_spaces = 0;
        while (*trimmed && isspace(*trimmed)) {
            trimmed++;
            leading_spaces++;
        }
        
        if (smart_indent) {
            for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
        }
        
        for (const char* p = trimmed; *p; p++) {
            if (*p == '"') {
                in_quotes = !in_quotes;
                fossil_io_puts("\033[32m");
                fossil_io_putchar(*p);
                fossil_io_puts("\033[0m");
            } else if (*p == ',' && !in_quotes) {
                fossil_io_puts("\033[33m");
                fossil_io_putchar(*p);
                fossil_io_puts("\033[0m");
            } else {
                fossil_io_putchar(*p);
            }
        }
        fossil_io_putchar('\n');
        
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
        char* trimmed = buffer;
        int leading_spaces = 0;
        while (*trimmed && isspace(*trimmed)) {
            trimmed++;
            leading_spaces++;
        }
        
        if (smart_indent) {
            for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
        }
        
        for (const char* p = trimmed; *p; p++) {
            if (*p == '\t') {
                fossil_io_puts("\033[33m");
                fossil_io_putchar(*p);
                fossil_io_puts("\033[0m");
            } else {
                fossil_io_putchar(*p);
            }
        }
        fossil_io_putchar('\n');
        
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
            fossil_io_putchar('\n');
            ptr = line_end;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        if (smart_indent && (*trimmed == '}' || *trimmed == ']')) {
            indent = (indent > 0) ? indent - 1 : 0;
        }
        
        if (smart_indent) {
            for (int i = 0; i < indent * 2; i++) fossil_io_putchar(' ');
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
                fossil_io_puts("\033[35m");
                while (*p && *p != ':') fossil_io_putchar(*(p++));
                fossil_io_putchar(':');
                fossil_io_puts("\033[0m");
                continue;
            }
            
            if (*p == '"' && (p == trimmed || *(p-1) != '\\')) {
                in_string = !in_string;
                fossil_io_puts(in_string ? "\033[32m" : "\033[0m");
            }
            fossil_io_putchar(*p);
        }
        fossil_io_putchar('\n');
        
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
            fossil_io_puts(bold ? "\033[1m" : "\033[0m");
            p++;
            continue;
        }
        if (*p == '*' || *p == '_') {
            italic = !italic;
            fossil_io_puts(italic ? "\033[3m" : "\033[0m");
            continue;
        }
        if (*p == '`') {
            code = !code;
            fossil_io_puts(code ? "\033[36m" : "\033[0m");
            continue;
        }
        fossil_io_putchar(*p);
    }
    fossil_io_puts("\033[0m");
}

static int fossil_markdown_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    int heading_level = 0;
    char buffer[2048];
    const char* ptr = content;
    bool in_code_block = false;
    
    while (sscanf(ptr, "%2047[^\n]", buffer) > 0) {
        char* trimmed = buffer;
        int leading_spaces = 0;
        while (*trimmed && isspace(*trimmed)) {
            trimmed++;
            leading_spaces++;
        }
        
        if (!*trimmed) {
            fossil_io_putchar('\n');
            ptr = strchr(ptr, '\n');
            if (!ptr) break;
            ptr++;
            continue;
        }
        
        // Handle code blocks
        if (strstr(trimmed, "```") != NULL) {
            in_code_block = !in_code_block;
            if (smart_indent) {
                for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
            }
            fossil_io_puts(trimmed);
            fossil_io_putchar('\n');
            ptr = strchr(ptr, '\n');
            if (!ptr) break;
            ptr++;
            continue;
        }
        
        // Handle headings
        if (*trimmed == '#') {
            heading_level = 0;
            while (heading_level < 6 && *(trimmed + heading_level) == '#') heading_level++;
            if (smart_indent) {
                for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
            }
            fossil_io_puts("\033[1m");
            fossil_io_puts(trimmed);
            fossil_io_puts("\033[0m\n");
            ptr = strchr(ptr, '\n');
            if (!ptr) break;
            ptr++;
            continue;
        }
        
        // Handle list indentation
        if (smart_indent) {
            if (*trimmed == '-' || *trimmed == '*' || *trimmed == '+' || 
                (isdigit(*trimmed) && strchr(trimmed, '.') && *(strchr(trimmed, '.') + 1) == ' ')) {
                for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
            } else {
                for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
            }
        }
        
        // Apply formatting if not in code block
        if (in_code_block) {
            fossil_io_puts(trimmed);
        } else {
            fossil_markdown_apply_emphasis(trimmed);
        }
        fossil_io_putchar('\n');
        
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
            if (smart_indent) {
                for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
            }
            fossil_io_puts(trimmed);
            fossil_io_putchar('\n');
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
        if (smart_indent) {
            for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
        }
        
        if (in_code_block) {
            fossil_io_puts("\033[36m");
        }
        
        fossil_io_puts(trimmed);
        if (in_code_block) fossil_io_puts("\033[0m");
        fossil_io_putchar('\n');
        
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
        int leading_spaces = 0;
        while (*trimmed && isspace(*trimmed)) {
            trimmed++;
            leading_spaces++;
        }
        
        // Handle code blocks
        if (strstr(trimmed, "----") != NULL || strstr(trimmed, "....") != NULL) {
            in_code = !in_code;
        }
        
        if (smart_indent) {
            for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
        }
        
        if (in_code) {
            fossil_io_puts("\033[36m");
        } else if (*trimmed == '=' && *(trimmed + 1) == ' ') {
            fossil_io_puts("\033[1m");
        }
        
        fossil_io_puts(trimmed);
        
        if (in_code || (*trimmed == '=' && *(trimmed + 1) == ' ')) {
            fossil_io_puts("\033[0m");
        }
        fossil_io_putchar('\n');
        
        ptr = strchr(ptr, '\n');
        if (!ptr) break;
        ptr++;
    }
    return 0;
}

static int fossil_plaintext_backend_format(const char* content, bool smart_indent) {
    if (!content) return -1;
    
    const char* ptr = content;
    char buffer[2048];
    
    while (sscanf(ptr, "%2047[^\n]", buffer) > 0) {
        char* trimmed = buffer;
        int leading_spaces = 0;
        
        while (*trimmed && isspace(*trimmed)) {
            trimmed++;
            leading_spaces++;
        }
        
        if (smart_indent) {
            for (int i = 0; i < leading_spaces; i++) fossil_io_putchar(' ');
        }
        
        fossil_io_puts(trimmed);
        fossil_io_putchar('\n');
        
        ptr = strchr(ptr, '\n');
        if (!ptr) break;
        ptr++;
    }
    
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

int fossil_shark_view(ccstring path, bool format) {
    if (!path) return -1;
    
    fossil_io_file_t stream = {0};
    if (fossil_io_file_open(&stream, path, "r") != 0) {
        return -1;
    }
    
    // Read entire file content
    fossil_io_file_seek(&stream, 0, SEEK_END);
    size_t file_size = fossil_io_file_get_size(&stream);
    fossil_io_file_seek(&stream, 0, SEEK_SET);
    
    char* content = malloc(file_size + 1);
    if (!content) {
        fossil_io_file_close(&stream);
        return -1;
    }
    
    size_t read_size = fossil_io_file_read(&stream, content, 1, file_size);
    content[read_size] = '\0';
    fossil_io_file_close(&stream);
    
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
            fossil_io_puts(content);
        }
    } else {
        fossil_io_puts(content);
    }
    
    free(content);
    return 0;
}
