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
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#define INDENT_SIZE 4

// ========================================================
// File and Directory Commands
// ========================================================

#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #define PATH_SEPARATOR '\\'
    #define PATH_SEPARATOR_STR "\\"
#else
    #include <sys/stat.h>
    #include <unistd.h>
    #include <dirent.h>
    #define PATH_SEPARATOR '/'
    #define PATH_SEPARATOR_STR "/"
#endif

static void print_size(off_t size, bool human_readable) {
    if (!human_readable) {
        fossil_io_printf("%lld ", (long long)size);
        return;
    }
    ccstring units[] = {"B", "KB", "MB", "GB", "TB"};
    double sz = (double)size;
    int i = 0;
    while (sz >= 1024 && i < 4) { sz /= 1024; i++; }
    fossil_io_printf("{blue}%.1f%s{normal} ", sz, units[i]);
}

static void print_permissions(mode_t mode) {
#ifdef _WIN32
    // Windows simplified permissions
    fossil_io_printf("{blue}%c{normal}", (mode & _S_IFDIR) ? 'd' : '-');
    fossil_io_printf("{blue}%c%c%c{normal} ",
           (mode & _S_IREAD) ? 'r' : '-',
           (mode & _S_IWRITE) ? 'w' : '-',
           (mode & _S_IEXEC) ? 'x' : '-');
#else
    // Unix-style permissions
    fossil_io_printf("{blue}%c{normal}", S_ISDIR(mode) ? 'd' : '-');
    fossil_io_printf("{blue}%c%c%c%c%c%c%c%c%c{normal} ",
           mode & S_IRUSR ? 'r' : '-',
           mode & S_IWUSR ? 'w' : '-',
           mode & S_IXUSR ? 'x' : '-',
           mode & S_IRGRP ? 'r' : '-',
           mode & S_IWGRP ? 'w' : '-',
           mode & S_IXGRP ? 'x' : '-',
           mode & S_IROTH ? 'r' : '-',
           mode & S_IWOTH ? 'w' : '-',
           mode & S_IXOTH ? 'x' : '-');
#endif
}

static void print_entry(ccstring path, ccstring name, struct stat *st,
                        bool long_format, bool human_readable, bool show_time,
                        ccstring prefix) {
    if (cnotnull(prefix))
        fossil_io_printf("{blue}%s{normal}", prefix);

    if (long_format) {
        print_permissions(st->st_mode);
        print_size(st->st_size, human_readable);

        if (show_time) {
            char *tbuf = fossil_sys_memory_alloc(64);
            if (cunlikely(!tbuf)) {
                fossil_io_fprintf(FOSSIL_STDERR, "Memory allocation failed\n");
                return;
            }
            struct tm *tm_info = localtime(&st->st_mtime);
            strftime(tbuf, 64, "%Y-%m-%d %H:%M:%S", tm_info);
            fossil_io_printf("{blue}%s{normal} ", tbuf);
            fossil_sys_memory_free(tbuf);
            cnullify(tbuf);
        }
    }

    if (cnotnull(path) && *path != '\0')
        fossil_io_printf("{blue}%s%c%s{normal}\n", path, PATH_SEPARATOR, name);
    else
        fossil_io_printf("{blue}%s{normal}\n", name);
}

#ifdef _WIN32
static int browse_directory_interactive_win(ccstring path) {
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind;
    char searchPattern[MAX_PATH];
    
    snprintf(searchPattern, MAX_PATH, "%s\\*", path);
    hFind = FindFirstFileA(searchPattern, &findFileData);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        fossil_io_fprintf(FOSSIL_STDERR, "Error opening directory: %s\n", path);
        return GetLastError();
    }

    char **choices = fossil_sys_memory_alloc(256 * sizeof(char*));
    if (cunlikely(!choices)) {
        FindClose(hFind);
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    
    int num_choices = 0;
    
    do {
        if (strcmp(findFileData.cFileName, ".") == 0) continue;
        if (num_choices >= 255) break;
        
        choices[num_choices] = fossil_sys_memory_strdup(findFileData.cFileName);
        if (cunlikely(!choices[num_choices])) {
            for (int i = 0; i < num_choices; i++) {
                fossil_sys_memory_free(choices[i]);
                cnullify(choices[i]);
            }
            fossil_sys_memory_free(choices);
            FindClose(hFind);
            return ERROR_NOT_ENOUGH_MEMORY;
        }
        num_choices++;
    } while (FindNextFileA(hFind, &findFileData) != 0);
    
    FindClose(hFind);

    if (num_choices == 0) {
        fossil_io_printf("Directory is empty.\n");
        fossil_sys_memory_free(choices);
        cnullify(choices);
        return 0;
    }

    const char **menu_choices = fossil_sys_memory_alloc(256 * sizeof(char*));
    if (cunlikely(!menu_choices)) {
        for (int i = 0; i < num_choices; i++) {
            fossil_sys_memory_free(choices[i]);
            cnullify(choices[i]);
        }
        fossil_sys_memory_free(choices);
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    
    for (int i = 0; i < num_choices; i++) {
        menu_choices[i] = choices[i];
    }

    char *prompt = fossil_sys_memory_alloc(512);
    if (cunlikely(!prompt)) {
        for (int i = 0; i < num_choices; i++) {
            fossil_sys_memory_free(choices[i]);
            cnullify(choices[i]);
        }
        fossil_sys_memory_free(choices);
        fossil_sys_memory_free(menu_choices);
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    
    snprintf(prompt, 512, "Select file/directory in %s:", path);
    
    int selected = fossil_io_display_menu(prompt, menu_choices, num_choices);
    if (clikely(selected >= 0 && selected < num_choices)) {
        fossil_io_printf("Selected: %s\n", choices[selected]);
        
        cstring full_path = fossil_io_cstring_format("%s%c%s", path, PATH_SEPARATOR, choices[selected]);
        if (cnotnull(full_path)) {
            DWORD attrs = GetFileAttributesA(full_path);
            if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                char *input = fossil_sys_memory_alloc(256);
                if (cnotnull(input)) {
                    fossil_io_printf("Enter subdirectory? (y/n): ");
                    if (fossil_io_gets(input, 256) == 0) {
                        fossil_io_trim(input);
                        if (input[0] == 'y' || input[0] == 'Y') {
                            browse_directory_interactive_win(full_path);
                        }
                    }
                    fossil_sys_memory_free(input);
                    cnullify(input);
                }
            }
            fossil_io_cstring_free(full_path);
            cnullify(full_path);
        }
    }
    
    for (int i = 0; i < num_choices; i++) {
        fossil_sys_memory_free(choices[i]);
        cnullify(choices[i]);
    }
    fossil_sys_memory_free(choices);
    fossil_sys_memory_free(menu_choices);
    fossil_sys_memory_free(prompt);
    cnullify(choices);
    cnullify(menu_choices);
    cnullify(prompt);
    
    return 0;
}
#endif

static int browse_directory_interactive(ccstring path) {
#ifdef _WIN32
    return browse_directory_interactive_win(path);
#else
    DIR *dir = opendir(path);
    if (cunlikely(!dir)) {
        fossil_io_fprintf(FOSSIL_STDERR, "Error opening directory: %s\n", path);
        return errno;
    }

    char **choices = fossil_sys_memory_alloc(256 * sizeof(char*));
    if (cunlikely(!choices)) {
        closedir(dir);
        return ENOMEM;
    }
    
    int num_choices = 0;
    struct dirent *entry;
    
    while ((entry = readdir(dir)) != cnull && num_choices < 255) {
        if (strcmp(entry->d_name, ".") == 0) continue;
        choices[num_choices] = fossil_sys_memory_strdup(entry->d_name);
        if (cunlikely(!choices[num_choices])) {
            for (int i = 0; i < num_choices; i++) {
                fossil_sys_memory_free(choices[i]);
                cnullify(choices[i]);
            }
            fossil_sys_memory_free(choices);
            closedir(dir);
            return ENOMEM;
        }
        num_choices++;
    }
    closedir(dir);

    if (num_choices == 0) {
        fossil_io_printf("Directory is empty.\n");
        fossil_sys_memory_free(choices);
        cnullify(choices);
        return 0;
    }

    const char **menu_choices = fossil_sys_memory_alloc(256 * sizeof(char*));
    if (cunlikely(!menu_choices)) {
        for (int i = 0; i < num_choices; i++) {
            fossil_sys_memory_free(choices[i]);
            cnullify(choices[i]);
        }
        fossil_sys_memory_free(choices);
        return ENOMEM;
    }
    
    for (int i = 0; i < num_choices; i++) {
        menu_choices[i] = choices[i];
    }

    char *prompt = fossil_sys_memory_alloc(512);
    if (cunlikely(!prompt)) {
        for (int i = 0; i < num_choices; i++) {
            fossil_sys_memory_free(choices[i]);
            cnullify(choices[i]);
        }
        fossil_sys_memory_free(choices);
        fossil_sys_memory_free(menu_choices);
        return ENOMEM;
    }
    
    snprintf(prompt, 512, "Select file/directory in %s:", path);
    
    int selected = fossil_io_display_menu(prompt, menu_choices, num_choices);
    if (clikely(selected >= 0 && selected < num_choices)) {
        fossil_io_printf("Selected: %s\n", choices[selected]);
        
        cstring full_path = fossil_io_cstring_format("%s%c%s", path, PATH_SEPARATOR, choices[selected]);
        if (cnotnull(full_path)) {
            struct stat st;
            if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
                char *input = fossil_sys_memory_alloc(256);
                if (cnotnull(input)) {
                    fossil_io_printf("Enter subdirectory? (y/n): ");
                    if (fossil_io_gets(input, 256) == 0) {
                        fossil_io_trim(input);
                        if (input[0] == 'y' || input[0] == 'Y') {
                            browse_directory_interactive(full_path);
                        }
                    }
                    fossil_sys_memory_free(input);
                    cnullify(input);
                }
            }
            fossil_io_cstring_free(full_path);
            cnullify(full_path);
        }
    }
    
    for (int i = 0; i < num_choices; i++) {
        fossil_sys_memory_free(choices[i]);
        cnullify(choices[i]);
    }
    fossil_sys_memory_free(choices);
    fossil_sys_memory_free(menu_choices);
    fossil_sys_memory_free(prompt);
    cnullify(choices);
    cnullify(menu_choices);
    cnullify(prompt);
    
    return 0;
#endif
}

static int show_tree_with_progress(ccstring path, bool show_all, bool long_format,
                                  bool human_readable, bool show_time,
                                  int depth, int current_depth, ccstring prefix) {
#ifdef _WIN32
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind;
    char searchPattern[MAX_PATH];
    
    snprintf(searchPattern, MAX_PATH, "%s\\*", path);
    hFind = FindFirstFileA(searchPattern, &findFileData);
    
    if (hFind == INVALID_HANDLE_VALUE) return GetLastError();

    int total_entries = 0;
    do {
        if (!show_all && findFileData.cFileName[0] == '.') continue;
        total_entries++;
    } while (FindNextFileA(hFind, &findFileData) != 0);
    
    FindClose(hFind);
    hFind = FindFirstFileA(searchPattern, &findFileData);
    
    int processed = 0;
    do {
        if (!show_all && findFileData.cFileName[0] == '.') continue;

        cstring full_path = fossil_io_cstring_format("%s%c%s", path, PATH_SEPARATOR, findFileData.cFileName);
        if (cunlikely(!full_path)) continue;
        
        struct stat st;
        if (stat(full_path, &st) != 0) {
            fossil_io_cstring_free(full_path);
            cnullify(full_path);
            continue;
        }

        print_entry(full_path, findFileData.cFileName, &st, long_format, human_readable, show_time, prefix);

        if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && 
            strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0 && 
            (depth == 0 || current_depth < depth)) {
            cstring child_prefix = fossil_io_cstring_format("%s│   ", cunwrap_or(prefix, ""));
            if (cnotnull(child_prefix)) {
                show_tree_with_progress(full_path, show_all, long_format, human_readable, show_time,
                                       depth, current_depth + 1, child_prefix);
                fossil_io_cstring_free(child_prefix);
                cnullify(child_prefix);
            }
        }

        processed++;
        if (clikely(total_entries > 10)) {
            int progress = (processed * 100) / total_entries;
            fossil_io_show_progress(progress);
        }

        fossil_io_cstring_free(full_path);
        cnullify(full_path);
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
    return 0;
#else
    DIR *dir = opendir(path);
    if (cunlikely(!dir)) return errno;

    int total_entries = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != cnull) {
        if (!show_all && entry->d_name[0] == '.') continue;
        total_entries++;
    }
    rewinddir(dir);

    int processed = 0;
    while ((entry = readdir(dir)) != cnull) {
        if (!show_all && entry->d_name[0] == '.') continue;

        cstring full_path = fossil_io_cstring_format("%s%c%s", path, PATH_SEPARATOR, entry->d_name);
        if (cunlikely(!full_path)) continue;
        
        struct stat st;
        if (stat(full_path, &st) != 0) {
            fossil_io_cstring_free(full_path);
            cnullify(full_path);
            continue;
        }

        print_entry(full_path, entry->d_name, &st, long_format, human_readable, show_time, prefix);

        if (S_ISDIR(st.st_mode) && strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0 && (depth == 0 || current_depth < depth)) {
            cstring child_prefix = fossil_io_cstring_format("%s│   ", cunwrap_or(prefix, ""));
            if (cnotnull(child_prefix)) {
                show_tree_with_progress(full_path, show_all, long_format, human_readable, show_time,
                                       depth, current_depth + 1, child_prefix);
                fossil_io_cstring_free(child_prefix);
                cnullify(child_prefix);
            }
        }

        processed++;
        if (clikely(total_entries > 10)) {
            int progress = (processed * 100) / total_entries;
            fossil_io_show_progress(progress);
        }

        fossil_io_cstring_free(full_path);
        cnullify(full_path);
    }

    closedir(dir);
    return 0;
#endif
}

static int show_tree(ccstring path, bool show_all, bool long_format,
                     bool human_readable, bool show_time,
                     int depth, int current_depth, ccstring prefix) {
#ifdef _WIN32
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind;
    char searchPattern[MAX_PATH];
    
    snprintf(searchPattern, MAX_PATH, "%s\\*", path);
    hFind = FindFirstFileA(searchPattern, &findFileData);
    
    if (hFind == INVALID_HANDLE_VALUE) return GetLastError();

    do {
        if (!show_all && findFileData.cFileName[0] == '.') continue;

        cstring full_path = fossil_io_cstring_format("%s%c%s", path, PATH_SEPARATOR, findFileData.cFileName);
        if (cunlikely(!full_path)) continue;
        
        struct stat st;
        if (stat(full_path, &st) != 0) {
            fossil_io_cstring_free(full_path);
            cnullify(full_path);
            continue;
        }

        cstring new_prefix = fossil_io_cstring_format("%s├── ", cunwrap_or(prefix, ""));
        if (cunlikely(!new_prefix)) {
            fossil_io_cstring_free(full_path);
            cnullify(full_path);
            continue;
        }

        print_entry(full_path, findFileData.cFileName, &st, long_format, human_readable, show_time, prefix);

        if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && 
            strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0 && 
            (depth == 0 || current_depth < depth)) {
            cstring child_prefix = fossil_io_cstring_format("%s│   ", cunwrap_or(prefix, ""));
            if (cnotnull(child_prefix)) {
                show_tree(full_path, show_all, long_format, human_readable, show_time,
                          depth, current_depth + 1, child_prefix);
                fossil_io_cstring_free(child_prefix);
                cnullify(child_prefix);
            }
        }

        fossil_io_cstring_free(new_prefix);
        fossil_io_cstring_free(full_path);
        cnullify(new_prefix);
        cnullify(full_path);
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
    return 0;
#else
    DIR *dir = opendir(path);
    if (cunlikely(!dir)) return errno;

    struct dirent *entry;
    while ((entry = readdir(dir)) != cnull) {
        if (!show_all && entry->d_name[0] == '.') continue;

        cstring full_path = fossil_io_cstring_format("%s%c%s", path, PATH_SEPARATOR, entry->d_name);
        if (cunlikely(!full_path)) continue;
        
        struct stat st;
        if (stat(full_path, &st) != 0) {
            fossil_io_cstring_free(full_path);
            cnullify(full_path);
            continue;
        }

        cstring new_prefix = fossil_io_cstring_format("%s├── ", cunwrap_or(prefix, ""));
        if (cunlikely(!new_prefix)) {
            fossil_io_cstring_free(full_path);
            cnullify(full_path);
            continue;
        }

        print_entry(full_path, entry->d_name, &st, long_format, human_readable, show_time, prefix);

        if (S_ISDIR(st.st_mode) && strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0 && (depth == 0 || current_depth < depth)) {
            cstring child_prefix = fossil_io_cstring_format("%s│   ", cunwrap_or(prefix, ""));
            if (cnotnull(child_prefix)) {
                show_tree(full_path, show_all, long_format, human_readable, show_time,
                          depth, current_depth + 1, child_prefix);
                fossil_io_cstring_free(child_prefix);
                cnullify(child_prefix);
            }
        }

        fossil_io_cstring_free(new_prefix);
        fossil_io_cstring_free(full_path);
        cnullify(new_prefix);
        cnullify(full_path);
    }

    closedir(dir);
    return 0;
#endif
}

static int show_graph(ccstring path, bool show_all, bool long_format,
                      bool human_readable, bool show_time,
                      int depth, int current_depth, int indent) {
#ifdef _WIN32
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind;
    char searchPattern[MAX_PATH];
    
    snprintf(searchPattern, MAX_PATH, "%s\\*", path);
    hFind = FindFirstFileA(searchPattern, &findFileData);
    
    if (hFind == INVALID_HANDLE_VALUE) return GetLastError();

    do {
        if (!show_all && findFileData.cFileName[0] == '.') continue;

        cstring full_path = fossil_io_cstring_format("%s%c%s", path, PATH_SEPARATOR, findFileData.cFileName);
        if (cunlikely(!full_path)) continue;
        
        struct stat st;
        if (stat(full_path, &st) != 0) {
            fossil_io_cstring_free(full_path);
            cnullify(full_path);
            continue;
        }

        for (int i = 0; i < indent; i++) fossil_io_printf("  ");
        print_entry(full_path, findFileData.cFileName, &st, long_format, human_readable, show_time, cnull);

        if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && 
            strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0 && 
            (depth == 0 || current_depth < depth)) {
            show_graph(full_path, show_all, long_format, human_readable, show_time,
                       depth, current_depth + 1, indent + 1);
        }

        fossil_io_cstring_free(full_path);
        cnullify(full_path);
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
    return 0;
#else
    DIR *dir = opendir(path);
    if (cunlikely(!dir)) return errno;

    struct dirent *entry;
    while ((entry = readdir(dir)) != cnull) {
        if (!show_all && entry->d_name[0] == '.') continue;

        cstring full_path = fossil_io_cstring_format("%s%c%s", path, PATH_SEPARATOR, entry->d_name);
        if (cunlikely(!full_path)) continue;
        
        struct stat st;
        if (stat(full_path, &st) != 0) {
            fossil_io_cstring_free(full_path);
            cnullify(full_path);
            continue;
        }

        for (int i = 0; i < indent; i++) fossil_io_printf("  ");
        print_entry(full_path, entry->d_name, &st, long_format, human_readable, show_time, cnull);

        if (S_ISDIR(st.st_mode) && strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0 && (depth == 0 || current_depth < depth)) {
            show_graph(full_path, show_all, long_format, human_readable, show_time,
                       depth, current_depth + 1, indent + 1);
        }

        fossil_io_cstring_free(full_path);
        cnullify(full_path);
    }

    closedir(dir);
    return 0;
#endif
}

int fossil_shark_show(ccstring path, bool show_all, bool long_format,
                      bool human_readable, bool recursive,
                      ccstring format, bool show_time, int depth) {
    if (cunlikely(!path)) path = ".";

    char *sanitized_path = fossil_sys_memory_alloc(1024);
    if (cunlikely(!sanitized_path)) {
        return ENOMEM;
    }
    
    int sanitize_result = fossil_io_validate_sanitize_string(path, sanitized_path, 
                                                            1024, 
                                                            FOSSIL_CTX_FILENAME);
    if (sanitize_result & (FOSSIL_SAN_PATH | FOSSIL_SAN_SCRIPT | FOSSIL_SAN_SHELL)) {
        fossil_io_fprintf(FOSSIL_STDERR, "Suspicious path detected, using sanitized version\n");
        path = sanitized_path;
    }

    int effective_depth = recursive ? depth : 1;

    int result = 0;
    if (cunlikely(!format) || fossil_io_cstring_equals(format, "list")) {
        result = show_graph(path, show_all, long_format, human_readable, show_time, effective_depth, 0, 0);
    } else if (fossil_io_cstring_equals(format, "tree")) {
        result = show_tree(path, show_all, long_format, human_readable, show_time, effective_depth, 0, cempty);
    } else if (fossil_io_cstring_equals(format, "graph")) {
        result = show_graph(path, show_all, long_format, human_readable, show_time, effective_depth, 0, 0);
    } else if (fossil_io_cstring_equals(format, "interactive")) {
        result = browse_directory_interactive(path);
    } else if (fossil_io_cstring_equals(format, "progress")) {
        result = show_tree_with_progress(path, show_all, long_format, human_readable, show_time, effective_depth, 0, cempty);
    } else {
        fossil_io_fprintf(FOSSIL_STDERR, "{blue}Unknown format: %s{normal}\n", format);
        result = 1;
    }

    fossil_sys_memory_free(sanitized_path);
    cnullify(sanitized_path);
    return result;
}
