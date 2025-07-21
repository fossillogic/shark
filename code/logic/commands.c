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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h> // For getcwd

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#include <windows.h> // Include for ULARGE_INTEGER and GetDiskFreeSpaceEx
#else
#include <sys/statvfs.h>
#endif

int ENABLE_VERBOSE = 0; // Global variable to control verbose output

int fossil_io_cstring_icmp(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (tolower((unsigned char)*str1) != tolower((unsigned char)*str2)) {
            return 0; // Not equal
        }
        str1++;
        str2++;
    }
    return (*str1 == '\0' && *str2 == '\0'); // Both strings must end at the same time
}

int fossil_io_cstring_icontains(ccstring str, ccstring substr) {
    const char *p = str;
    while (*p) {
        if (fossil_io_cstring_icmp(p, substr) == 1) {
            return 1; // Found
        }
        p++;
    }
    return 0; // Not found
}

void fossil_fstream_rewind(fossil_fstream_t *stream) {
    if (stream && stream->file) {
        rewind(stream->file);
    }
}

int fossil_io_gets(char *buffer, size_t size) {
    if (fgets(buffer, size, stdin) == NULL) {
        return -1; // Error or EOF
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0'; // Remove newline character
    }
    return 0; // Success
}



// Handler function definitions
void handle_help(void) {
    fossil_io_printf("{blue}Usage: shark <command> [options]{reset}\n");
    fossil_io_printf("{blue}Available commands and flags:{reset}\n");
    fossil_io_printf("  {cyan}move{reset}      --force, --interactive      Move or rename files and directories\n");
    fossil_io_printf("  {cyan}rename{reset}    --force                   Rename files or directories\n");
    fossil_io_printf("  {cyan}copy{reset}      --force, --recursive      Copy files or directories\n");
    fossil_io_printf("  {cyan}delete{reset}    --force, --recursive, --interactive   Delete files or directories\n");
    fossil_io_printf("  {cyan}list{reset}      --sort, --format, --all, --long       List files and directories\n");
    fossil_io_printf("  {cyan}show{reset}      --head, --tail, --lines               Display contents of a file\n");
    fossil_io_printf("  {cyan}find{reset}      --name, --size, --type, --hidden      Find files matching criteria\n");
    fossil_io_printf("  {cyan}where{reset}     --name, --type                        Show location of files or directories\n");
    fossil_io_printf("  {cyan}search{reset}    --pattern, --ignore-case, --count     Search file contents for patterns\n");
    fossil_io_printf("  {cyan}ask{reset}       --message, --default, --timeout       Prompt for user input\n");
    fossil_io_printf("  {cyan}edit{reset}      --editor, --backup                    Edit files from the command line\n");
    fossil_io_printf("  {cyan}size{reset}      --human-readable, --total             Display size of files or directories\n");
    fossil_io_printf("  {cyan}disk{reset}      --all, --human-readable               Display disk usage and free space\n");
    fossil_io_printf("  {cyan}tree{reset}      --depth, --all, --color               Display directory structure as a tree\n");
    fossil_io_printf("  {cyan}compare{reset}   --recursive, --brief, --ignore-case   Compare two files or directories\n");
    fossil_io_printf("  {cyan}create{reset}    --directory, --parents                Create new files or directories\n");
    fossil_io_printf("  {cyan}color{reset}     enable, disable, auto                 Set color output mode\n");
    fossil_io_printf("  {cyan}clear{reset}     --force                               Clear output from console\n");
    fossil_io_printf("\n{blue}Global flags:{reset}\n");
    fossil_io_printf("  {cyan}--version{reset}   Display the current version\n");
    fossil_io_printf("  {cyan}--help{reset}      Display help information\n");
    fossil_io_printf("  {cyan}--name{reset}      Display app name\n");
    fossil_io_printf("  {cyan}--verbose{reset}   Enable verbose output\n");
    fossil_io_printf("  {cyan}--clear{reset}     Clear output from the console\n");
    fossil_io_flush();
}

void handle_version(void) {
    fossil_io_printf("shark version %s\n", FOSSIL_APP_VERSION);
}

void handle_name(void) {
    fossil_io_printf("Application name: %s\n", FOSSIL_APP_NAME);
}

void handle_move(const char *source, const char *destination, int force, int interactive) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Moving from '%s' to '%s'...{reset}\n", source, destination);
    }

    struct stat st;
    if (stat(source, &st) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Source file does not exist: %s{reset}\n", source);
        return;
    }

    int dest_exists = (stat(destination, &st) == 0);

    // If destination exists and not force, prompt or abort
    if (dest_exists && !force) {
        if (interactive) {
            fossil_io_printf("{yellow}Destination '%s' exists. Overwrite? (y/N): {reset}", destination);
            char answer[8];
            if (fossil_io_gets(answer, sizeof(answer)) == -1 || (answer[0] != 'y' && answer[0] != 'Y')) {
                fossil_io_printf("{cyan}Move cancelled by user.{reset}\n");
                return;
            }
        } else {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Destination exists: %s (use --force to overwrite){reset}\n", destination);
            return;
        }
    }

    if (fossil_fstream_rename(source, destination) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error moving file: %s{reset}\n", source);
    } else {
        if (ENABLE_VERBOSE) {
            fossil_io_printf("{cyan}Moved '%s' to '%s' successfully.{reset}\n", source, destination);
        }
    }
}

void handle_copy(const char *source, const char *destination, int force, int interactive) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Copying from '%s' to '%s'...{reset}\n", source, destination);
    }

    struct stat st;
    int dest_exists = (stat(destination, &st) == 0);

    // If destination exists and not force, prompt or abort
    if (dest_exists && !force) {
        if (interactive) {
            fossil_io_printf("{yellow}Destination '%s' exists. Overwrite? (y/N): {reset}", destination);
            char answer[8];
            if (fossil_io_gets(answer, sizeof(answer)) == -1 || (answer[0] != 'y' && answer[0] != 'Y')) {
                fossil_io_printf("{cyan}Copy cancelled by user.{reset}\n");
                return;
            }
        } else {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Destination exists: %s (use --force to overwrite){reset}\n", destination);
            return;
        }
    }

    if (fossil_fstream_copy(source, destination) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error copying file: %s{reset}\n", source);
    } else {
        if (ENABLE_VERBOSE) {
            fossil_io_printf("{cyan}Copied '%s' to '%s' successfully.{reset}\n", source, destination);
        }
    }
}

void handle_delete(const char *target, int force, int recursive, int interactive) {
    struct stat st;
    if (stat(target, &st) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Target does not exist: %s{reset}\n", target);
        return;
    }

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Deleting '%s'...{reset}\n", target);
    }

    // If interactive, prompt before deleting
    if (interactive) {
        fossil_io_printf("{yellow}Delete '%s'? (y/N): {reset}", target);
        char answer[8];
        if (fossil_io_gets(answer, sizeof(answer)) == -1 || (answer[0] != 'y' && answer[0] != 'Y')) {
            fossil_io_printf("{cyan}Delete cancelled by user.{reset}\n");
            return;
        }
    }

    // If directory and recursive is not set, abort
    if (S_ISDIR(st.st_mode)) {
        if (!recursive) {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Cannot delete directory without --recursive: %s{reset}\n", target);
            return;
        }
        // Recursively delete directory contents
        DIR *dir = opendir(target);
        if (dir == cnull) {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening directory: %s{reset}\n", target);
            return;
        }
        struct dirent *entry;
        char path[1024];
        while ((entry = readdir(dir)) != cnull) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", target, entry->d_name);
            handle_delete(path, force, recursive, interactive);
        }
        closedir(dir);
        // Remove the directory itself
        if (rmdir(target) != 0) {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error deleting directory: %s{reset}\n", target);
            return;
        }
    } else {
        // If not force and file is write-protected, abort
        if (!force && access(target, W_OK) != 0) {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}File is write-protected: %s (use --force to override){reset}\n", target);
            return;
        }
        if (fossil_fstream_delete(target) != 0) {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error deleting file: %s{reset}\n", target);
            return;
        }
    }

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Deleted '%s' successfully.{reset}\n", target);
    }
}

int custom_list(const char *directory) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(directory);
    if (dir == cnull) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening directory: %s{reset}\n", directory);
        return -1; // Return non-zero on failure
    }

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Opening directory '%s'...{reset}\n", directory);
    }

    fossil_io_printf("{cyan}Contents of '%s':{reset}\n", directory);
    while ((entry = readdir(dir)) != cnull) {
#ifdef _WIN32
        // On Windows, skip "." and ".." explicitly
        if (fossil_io_cstring_compare(entry->d_name, ".") == 0 || fossil_io_cstring_compare(entry->d_name, "..") == 0) {
            continue;
        }
#endif
        fossil_io_printf("{cyan} -> {blue}%s{reset}\n", entry->d_name);
    }

    closedir(dir);
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Closed directory '%s'.{reset}\n", directory);
    }
    return 0; // Return 0 on success
}

void handle_list(const char *directory, const char *sort, const char *format, int all, int long_format) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Listing contents of directory '%s'...{reset}\n", directory);
        fossil_io_printf("{cyan}Options: sort='%s', format='%s', all=%d, long_format=%d{reset}\n",
                         sort ? sort : "(none)", format ? format : "(none)", all, long_format);
    }

    DIR *dir = opendir(directory);
    if (dir == cnull) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening directory: %s{reset}\n", directory);
        return;
    }

    struct dirent *entry;
    struct stat st;
    char path[1024];

    // Simple array for sorting (not efficient for large dirs)
    char *names[1024];
    int count = 0;

    // Collect entries
    while ((entry = readdir(dir)) != cnull) {
        // Skip "." and ".." unless 'all' is set
        if (!all && (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0))
            continue;
        names[count++] = strdup(entry->d_name);
        if (count >= 1024) break;
    }
    closedir(dir);

    // Sort if requested (only by name for demo)
    if (sort && strcmp(sort, "name") == 0) {
        for (int i = 0; i < count - 1; ++i) {
            for (int j = i + 1; j < count; ++j) {
                if (strcmp(names[i], names[j]) > 0) {
                    char *tmp = names[i];
                    names[i] = names[j];
                    names[j] = tmp;
                }
            }
        }
    }

    // Print entries
    for (int i = 0; i < count; ++i) {
        snprintf(path, sizeof(path), "%s/%s", directory, names[i]);
        if (long_format && stat(path, &st) == 0) {
            fossil_io_printf("{cyan} -> {blue}%s{reset}  [size: %lld bytes, type: %s]\n",
                names[i],
                (long long)st.st_size,
                S_ISDIR(st.st_mode) ? "DIR" : "FILE");
        } else if (format && strcmp(format, "json") == 0) {
            fossil_io_printf("{cyan}{\"name\": \"%s\"}{reset}\n", names[i]);
        } else {
            fossil_io_printf("{cyan} -> {blue}%s{reset}\n", names[i]);
        }
        free(names[i]);
    }

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Listed contents of '%s' successfully.{reset}\n", directory);
    }
}

void handle_show(const char *file, int head, int tail, const char *lines) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Displaying contents of file '%s'...{reset}\n", file);
    }
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, file, "r") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening file: %s{reset}\n", file);
        return;
    }

    int max_lines = -1;
    if (lines && strlen(lines) > 0) {
        max_lines = atoi(lines);
    }

    char buffer[256];
    int total_lines = 0;
    int print_start = 0, print_end = 0;

    // First, count total lines if tail is requested
    if (tail > 0 || (max_lines > 0 && head <= 0)) {
        while (fossil_io_gets_from_stream(buffer, sizeof(buffer), &stream) != cnull) {
            total_lines++;
        }
        fossil_fstream_rewind(&stream);
    }

    // Determine which lines to print
    if (head > 0) {
        print_start = 1;
        print_end = (max_lines > 0 && max_lines < head) ? max_lines : head;
    } else if (tail > 0) {
        print_start = total_lines - tail + 1;
        if (print_start < 1) print_start = 1;
        print_end = total_lines;
    } else if (max_lines > 0) {
        print_start = 1;
        print_end = max_lines;
    } else {
        print_start = 1;
        print_end = INT_MAX;
    }

    int current_line = 0;
    while (fossil_io_gets_from_stream(buffer, sizeof(buffer), &stream) != cnull) {
        current_line++;
        if (current_line >= print_start && current_line <= print_end) {
            fossil_io_printf("%s\n", buffer);
        }
        if (current_line >= print_end) break;
    }

    fossil_fstream_close(&stream);
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished displaying contents of file '%s'.{reset}\n", file);
    }
}

void handle_find(const char *directory, const char *pattern, const char *size, const char *type, int hidden) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finding files in '%s' matching pattern '%s', size='%s', type='%s', hidden=%d...{reset}\n",
                         directory, pattern ? pattern : "(none)", size ? size : "(none)", type ? type : "(none)", hidden);
    }

    DIR *dir;
    struct dirent *entry;

    dir = opendir(directory);
    if (dir == cnull) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening directory: %s{reset}\n", directory);
        return;
    }

    while ((entry = readdir(dir)) != cnull) {
        // Skip "." and ".."
        if (fossil_io_cstring_compare(entry->d_name, ".") == 0 || fossil_io_cstring_compare(entry->d_name, "..") == 0) {
            continue;
        }

        // Hidden file check (on Unix: starts with '.', on Windows: skip for demo)
        int is_hidden = entry->d_name[0] == '.';
        if (!hidden && is_hidden) continue;

        // Type check
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
        struct stat st;
        if (stat(path, &st) != 0) continue;
        if (type) {
            if (strcmp(type, "dir") == 0 && !S_ISDIR(st.st_mode)) continue;
            if (strcmp(type, "file") == 0 && !S_ISREG(st.st_mode)) continue;
        }

        // Size check (simple: supports "min:bytes" or "max:bytes")
        if (size && strlen(size) > 0) {
            long long file_size = (long long)st.st_size;
            if (strncmp(size, "min:", 4) == 0) {
                long long min_size = atoll(size + 4);
                if (file_size < min_size) continue;
            } else if (strncmp(size, "max:", 4) == 0) {
                long long max_size = atoll(size + 4);
                if (file_size > max_size) continue;
            }
        }

        // Pattern check
        if (pattern && !fossil_io_cstring_contains(entry->d_name, pattern)) continue;

        fossil_io_printf("{cyan}  %s{reset}\n", entry->d_name);
    }

    closedir(dir);
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished searching in '%s'.{reset}\n", directory);
    }
}

void handle_search(const char *file, const char *pattern, int ignore_case, int count) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Searching file '%s' for pattern '%s'...{reset}\n", file, pattern);
        fossil_io_printf("{cyan}Options: ignore_case=%d, count=%d{reset}\n", ignore_case, count);
    }

    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, file, "r") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening file: %s{reset}\n", file);
        return;
    }

    char line[256];
    int line_number = 0;
    int match_count = 0;
    while (fossil_io_gets_from_stream(line, sizeof(line), &stream) != cnull) {
        line_number++;
        int found = 0;
        if (ignore_case) {
            found = fossil_io_cstring_icontains(line, pattern); // Assume this function exists for case-insensitive search
        } else {
            found = fossil_io_cstring_contains(line, pattern);
        }
        if (found) {
            match_count++;
            if (!count) {
                fossil_io_printf("{cyan}Line %d: %s{reset}\n", line_number, line);
            }
        }
    }
    fossil_fstream_close(&stream);

    if (count) {
        fossil_io_printf("{cyan}Total matches: %d{reset}\n", match_count);
    }

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished searching file '%s'.{reset}\n", file);
    }
}

void handle_size(const char *target, int human_readable, int total) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{blue,bold}Calculating size of '%s'...{reset}\n", target);
    }

    struct stat st;
    if (stat(target, &st) == 0) {
        long long size_bytes = (long long)st.st_size;

        if (human_readable) {
            double size = (double)size_bytes;
            const char *units[] = {"bytes", "KB", "MB", "GB", "TB"};
            int unit = 0;
            while (size >= 1024 && unit < 4) {
                size /= 1024;
                unit++;
            }
            fossil_io_printf("{blue,bold}Size of '%s':{reset}\n", target);
            fossil_io_printf("{cyan}  %.2f %s{reset}\n", size, units[unit]);
        } else {
            fossil_io_printf("{blue,bold}Size of '%s':{reset}\n", target);
            fossil_io_printf("{cyan}  %lld bytes{reset}\n", size_bytes);
        }

        if (total && S_ISDIR(st.st_mode)) {
            // Calculate total size of directory recursively
            long long dir_total = 0;
            DIR *dir = opendir(target);
            if (dir != cnull) {
                struct dirent *entry;
                char path[1024];
                while ((entry = readdir(dir)) != cnull) {
                    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                        continue;
                    snprintf(path, sizeof(path), "%s/%s", target, entry->d_name);
                    struct stat entry_st;
                    if (stat(path, &entry_st) == 0) {
                        if (S_ISDIR(entry_st.st_mode)) {
                            // Recursively add subdirectory size
                            handle_size(path, 0, 1); // Not human_readable for recursion
                        } else {
                            dir_total += (long long)entry_st.st_size;
                        }
                    }
                }
                closedir(dir);
            }
            if (human_readable) {
                double size = (double)dir_total;
                int unit = 0;
                const char *units[] = {"bytes", "KB", "MB", "GB", "TB"};
                while (size >= 1024 && unit < 4) {
                    size /= 1024;
                    unit++;
                }
                fossil_io_printf("{blue,bold}Total size of directory '%s':{reset}\n", target);
                fossil_io_printf("{cyan}  %.2f %s{reset}\n", size, units[unit]);
            } else {
                fossil_io_printf("{blue,bold}Total size of directory '%s':{reset}\n", target);
                fossil_io_printf("{cyan}  %lld bytes{reset}\n", dir_total);
            }
        }
    } else {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error getting size of '%s': %s{reset}\n", target, strerror(errno));
    }

    cunused(target); // To avoid unused parameter warning

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{blue,bold}Finished calculating size of '%s'.{reset}\n", target);
    }
}

void handle_disk(const char *path, int all, int human_readable) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Calculating disk usage for '%s'...{reset}\n", path);
        fossil_io_printf("{cyan}Options: all=%d, human_readable=%d{reset}\n", all, human_readable);
    }

    if (path == NULL || strlen(path) == 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Invalid path provided.{reset}\n");
        return;
    }

#if defined(_WIN32) || defined(_WIN64)
    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    if (GetDiskFreeSpaceEx(path, &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
        unsigned long long total = totalBytes.QuadPart;
        unsigned long long free = totalFreeBytes.QuadPart;
        unsigned long long used = total - free;

        if (human_readable) {
            double t = (double)total, f = (double)free, u = (double)used;
            const char *units[] = {"bytes", "KB", "MB", "GB", "TB"};
            int tu = 0, fu = 0, uu = 0;
            while (t >= 1024 && tu < 4) { t /= 1024; tu++; }
            while (f >= 1024 && fu < 4) { f /= 1024; fu++; }
            while (u >= 1024 && uu < 4) { u /= 1024; uu++; }
            fossil_io_printf("{cyan}Disk Usage for '%s':{reset}\n", path);
            fossil_io_printf("  Total: %.2f %s\n  Free: %.2f %s\n  Used: %.2f %s\n", t, units[tu], f, units[fu], u, units[uu]);
        } else {
            fossil_io_printf("{cyan}Disk Usage for '%s':{reset}\n", path);
            fossil_io_printf("  Total: %llu bytes\n  Free: %llu bytes\n  Used: %llu bytes\n", total, free, used);
        }

        if (all) {
            fossil_io_printf("{cyan}  Free bytes available to user: %llu{reset}\n", freeBytesAvailable.QuadPart);
        }
    } else {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error getting disk usage: %s{reset}\n", path);
    }
#else
    struct statvfs stat;
    if (statvfs(path, &stat) == 0) {
        unsigned long long total = (unsigned long long)stat.f_frsize * stat.f_blocks;
        unsigned long long free = (unsigned long long)stat.f_frsize * stat.f_bfree;
        unsigned long long available = (unsigned long long)stat.f_frsize * stat.f_bavail;
        unsigned long long used = total - free;

        if (human_readable) {
            double t = (double)total, f = (double)free, a = (double)available, u = (double)used;
            const char *units[] = {"bytes", "KB", "MB", "GB", "TB"};
            int tu = 0, fu = 0, au = 0, uu = 0;
            while (t >= 1024 && tu < 4) { t /= 1024; tu++; }
            while (f >= 1024 && fu < 4) { f /= 1024; fu++; }
            while (a >= 1024 && au < 4) { a /= 1024; au++; }
            while (u >= 1024 && uu < 4) { u /= 1024; uu++; }
            fossil_io_printf("{cyan}Disk Usage for '%s':{reset}\n", path);
            fossil_io_printf("  Total: %.2f %s\n  Free: %.2f %s\n  Available: %.2f %s\n  Used: %.2f %s\n",
                t, units[tu], f, units[fu], a, units[au], u, units[uu]);
        } else {
            fossil_io_printf("{cyan}Disk Usage for '%s':{reset}\n", path);
            fossil_io_printf("  Total: %llu bytes\n  Free: %llu bytes\n  Available: %llu bytes\n  Used: %llu bytes\n",
                total, free, available, used);
        }

        if (all) {
            fossil_io_printf("{cyan}  Filesystem block size: %lu{reset}\n", stat.f_frsize);
            fossil_io_printf("{cyan}  Total blocks: %lu{reset}\n", stat.f_blocks);
            fossil_io_printf("{cyan}  Free blocks: %lu{reset}\n", stat.f_bfree);
            fossil_io_printf("{cyan}  Available blocks: %lu{reset}\n", stat.f_bavail);
        }
    } else {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error getting disk usage: %s{reset}\n", strerror(errno));
    }
#endif

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished calculating disk usage for '%s'.{reset}\n", path);
    }
}

void print_tree(const char *directory, int current_depth, int max_depth, int all, int color, const char *prefix) {
    if (max_depth >= 0 && current_depth > max_depth) return;

    DIR *dir = opendir(directory);
    if (dir == cnull) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening directory: %s{reset}\n", directory);
        return;
    }

    struct dirent *entry;
    int first = 1;
    while ((entry = readdir(dir)) != cnull) {
        // Skip "." and ".." unless 'all' is set
        if (!all && (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0))
            continue;

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);

        struct stat st;
        int is_dir = (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
        const char *branch = first ? "├── " : "│   ";
        first = 0;

        // Color output if requested
        if (color) {
            if (is_dir)
                fossil_io_printf("%s{blue}%s[DIR] %s{reset}\n", prefix, branch, entry->d_name);
            else
                fossil_io_printf("%s{green}%s%s{reset}\n", prefix, branch, entry->d_name);
        } else {
            if (is_dir)
                fossil_io_printf("%s%s[DIR] %s\n", prefix, branch, entry->d_name);
            else
                fossil_io_printf("%s%s%s\n", prefix, branch, entry->d_name);
        }

        // Recurse into subdirectories
        if (is_dir) {
            char new_prefix[1024];
            snprintf(new_prefix, sizeof(new_prefix), "%s│   ", prefix);
            print_tree(path, current_depth + 1, max_depth, all, color, new_prefix);
        }
    }
    closedir(dir);
}

void handle_tree(const char *directory, int depth, int all, int color) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Walking directory tree for '%s'...{reset}\n", directory);
        fossil_io_printf("{cyan}Options: depth=%d, all=%d, color=%d{reset}\n", depth, all, color);
    }
    // Print root directory
    if (color)
        fossil_io_printf("{blue}%s{reset}\n", directory);
    else
        fossil_io_printf("%s\n", directory);

    print_tree(directory, 1, depth, all, color, "");

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished walking directory tree for '%s'.{reset}\n", directory);
    }
}

void handle_compare(const char *path1, const char *path2, int recursive, int brief, int ignore_case) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Comparing '%s' with '%s'...{reset}\n", path1, path2);
        fossil_io_printf("{cyan}Options: recursive=%d, brief=%d, ignore_case=%d{reset}\n", recursive, brief, ignore_case);
    }

    struct stat st1, st2;
    if (stat(path1, &st1) != 0 || stat(path2, &st2) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error accessing files or directories: %s or %s{reset}\n", path1, path2);
        return;
    }

    // Directory comparison (recursive)
    if (recursive && S_ISDIR(st1.st_mode) && S_ISDIR(st2.st_mode)) {
        DIR *dir1 = opendir(path1);
        DIR *dir2 = opendir(path2);
        if (dir1 == cnull || dir2 == cnull) {
            if (dir1) closedir(dir1);
            if (dir2) closedir(dir2);
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening directories: %s or %s{reset}\n", path1, path2);
            return;
        }
        // Simple recursive compare: list entries in dir1, check existence and compare in dir2
        struct dirent *entry;
        while ((entry = readdir(dir1)) != cnull) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            char sub1[1024], sub2[1024];
            snprintf(sub1, sizeof(sub1), "%s/%s", path1, entry->d_name);
            snprintf(sub2, sizeof(sub2), "%s/%s", path2, entry->d_name);
            struct stat sub_st1, sub_st2;
            int exists1 = (stat(sub1, &sub_st1) == 0);
            int exists2 = (stat(sub2, &sub_st2) == 0);
            if (!exists2) {
                fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Missing in '%s': %s{reset}\n", path2, entry->d_name);
                if (brief) continue;
            } else if (!exists1) {
                fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Missing in '%s': %s{reset}\n", path1, entry->d_name);
                if (brief) continue;
            } else if (S_ISDIR(sub_st1.st_mode) && S_ISDIR(sub_st2.st_mode)) {
                handle_compare(sub1, sub2, recursive, brief, ignore_case);
            } else if (S_ISREG(sub_st1.st_mode) && S_ISREG(sub_st2.st_mode)) {
                handle_compare(sub1, sub2, 0, brief, ignore_case);
            } else {
                fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Type mismatch for '%s'{reset}\n", entry->d_name);
                if (brief) continue;
            }
        }
        closedir(dir1);
        closedir(dir2);
        if (!brief)
            fossil_io_printf("{cyan}Finished recursive directory compare.{reset}\n");
        return;
    }

    // File comparison
    fossil_fstream_t stream1, stream2;
    if (fossil_fstream_open(&stream1, path1, "r") != 0 || fossil_fstream_open(&stream2, path2, "r") != 0) {
        if (fossil_fstream_is_open(&stream1)) fossil_fstream_close(&stream1);
        if (fossil_fstream_is_open(&stream2)) fossil_fstream_close(&stream2);
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening files: %s or %s{reset}\n", path1, path2);
        return;
    }

    char buf1[256], buf2[256];
    int line_number = 0, diff_found = 0;
    while (fossil_io_gets_from_stream(buf1, sizeof(buf1), &stream1) != cnull &&
           fossil_io_gets_from_stream(buf2, sizeof(buf2), &stream2) != cnull) {
        line_number++;
        int cmp;
        if (ignore_case) {
            cmp = fossil_io_cstring_icmp(buf1, buf2); // Assume this function exists for case-insensitive compare
        } else {
            cmp = strcmp(buf1, buf2);
        }
        if (cmp != 0) {
            diff_found = 1;
            if (brief) {
                fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Files differ: %s and %s{reset}\n", path1, path2);
                break;
            } else {
                fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Difference at line %d:\n  %s: %s\n  %s: %s{reset}\n",
                    line_number, path1, buf1, path2, buf2);
            }
        }
    }

    // Check for extra lines
    if (!diff_found &&
        (fossil_io_gets_from_stream(buf1, sizeof(buf1), &stream1) != cnull ||
         fossil_io_gets_from_stream(buf2, sizeof(buf2), &stream2) != cnull)) {
        diff_found = 1;
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Files differ in size: %s and %s{reset}\n", path1, path2);
    }

    if (!diff_found) {
        fossil_io_printf("{cyan}Files are identical.{reset}\n");
    }

    fossil_fstream_close(&stream1);
    fossil_fstream_close(&stream2);

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished comparing '%s' with '%s'.{reset}\n", path1, path2);
    }
}

void handle_create(const char *target, int directory, int parents) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Creating '%s'...{reset}\n", target);
        fossil_io_printf("{cyan}Options: directory=%d, parents=%d{reset}\n", directory, parents);
    }

    int result = 0;

    if (directory) {
        // Create directory (with parents if requested)
        if (parents) {
            // Recursively create parent directories
            char path[1024];
            strncpy(path, target, sizeof(path));
            path[sizeof(path) - 1] = '\0';
            for (char *p = path + 1; *p; ++p) {
                if (*p == '/' || *p == '\\') {
                    char tmp = *p;
                    *p = '\0';
#if defined(_WIN32) || defined(_WIN64)
                    result = _mkdir(path);
#else
                    result = mkdir(path, 0755);
#endif
                    *p = tmp;
                }
            }
#if defined(_WIN32) || defined(_WIN64)
            result = _mkdir(path);
#else
            result = mkdir(path, 0755);
#endif
        } else {
            // Create only the final directory
#if defined(_WIN32) || defined(_WIN64)
            result = _mkdir(target);
#else
            result = mkdir(target, 0755);
#endif
        }
        if (result != 0 && errno != EEXIST) {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error creating directory: %s{reset}\n", target);
            return;
        }
        fossil_io_printf("{cyan}Created directory '%s' successfully.{reset}\n", target);
    } else {
        // Create file (parents if requested)
        if (parents) {
            // Create parent directories for file
            char dir_path[1024];
            strncpy(dir_path, target, sizeof(dir_path));
            dir_path[sizeof(dir_path) - 1] = '\0';
            char *slash = strrchr(dir_path, '/');
#if defined(_WIN32) || defined(_WIN64)
            if (!slash) slash = strrchr(dir_path, '\\');
#endif
            if (slash) {
                *slash = '\0';
                // Recursively create parent directories
                for (char *p = dir_path + 1; *p; ++p) {
                    if (*p == '/' || *p == '\\') {
                        char tmp = *p;
                        *p = '\0';
#if defined(_WIN32) || defined(_WIN64)
                        _mkdir(dir_path);
#else
                        mkdir(dir_path, 0755);
#endif
                        *p = tmp;
                    }
                }
#if defined(_WIN32) || defined(_WIN64)
                _mkdir(dir_path);
#else
                mkdir(dir_path, 0755);
#endif
            }
        }
        fossil_fstream_t stream;
        if (fossil_fstream_open(&stream, target, "w") != 0) {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error creating file: %s{reset}\n", target);
            return;
        }
        fossil_fstream_close(&stream);
        fossil_io_printf("{cyan}Created file '%s' successfully.{reset}\n", target);
    }

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished creating '%s'.{reset}\n", target);
    }
}

const char *handle_ask(const char *message, const char *default_response, int timeout) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Asking user: '%s' (default: '%s', timeout: %d seconds)...{reset}\n", message, default_response, timeout);
    }

    // Check for file/directory existence or type if message requests it
    if (strncmp(message, "exists:", 7) == 0) {
        const char *path = message + 7;
        struct stat st;
        if (stat(path, &st) == 0) {
            fossil_io_printf("{green}Exists: %s{reset}\n", path);
            return "yes";
        } else {
            fossil_io_printf("{red}Does not exist: %s{reset}\n", path);
            return "no";
        }
    } else if (strncmp(message, "type:", 5) == 0) {
        const char *path = message + 5;
        struct stat st;
        if (stat(path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                fossil_io_printf("{blue}Type: directory{reset}\n");
                return "directory";
            } else if (S_ISREG(st.st_mode)) {
                fossil_io_printf("{blue}Type: file{reset}\n");
                return "file";
            } else {
                fossil_io_printf("{yellow}Type: other{reset}\n");
                return "other";
            }
        } else {
            fossil_io_printf("{red}Does not exist: %s{reset}\n", path);
            return "not found";
        }
    }

    // Simulate user input (for demonstration purposes)
    const char *user_input = default_response;

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}User response: '%s'{reset}\n", user_input);
    }

    return user_input;
}

void handle_rename(const char *old_name, const char *new_name, int force) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Renaming '%s' to '%s'...{reset}\n", old_name, new_name);
    }

    struct stat st;
    int dest_exists = (stat(new_name, &st) == 0);

    // If destination exists and not force, abort
    if (dest_exists && !force) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Destination exists: %s (use --force to overwrite){reset}\n", new_name);
        return;
    }

    if (fossil_fstream_rename(old_name, new_name) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error renaming file: %s to %s{reset}\n", old_name, new_name);
    } else {
        fossil_io_printf("{cyan}Renamed '%s' to '%s' successfully.{reset}\n", old_name, new_name);
    }
}

void handle_where(const char *name, const char *type) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        fossil_io_printf("{cyan}Current working directory: %s{reset}\n", cwd);
        DIR *dir = opendir(cwd);
        if (dir != cnull) {
            struct dirent *entry;
            while ((entry = readdir(dir)) != cnull) {
                if (strcmp(entry->d_name, name) == 0) {
                    char path[1024];
                    int written = snprintf(path, sizeof(path), "%s/%s", cwd, entry->d_name);
                    if (written < 0 || written >= sizeof(path)) {
                        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Path too long, skipping: %s/%s{reset}\n", cwd, entry->d_name);
                        continue;
                    }
                    struct stat st;
                    if (stat(path, &st) == 0) {
                        int match_type = 1;
                        if (type) {
                            if (strcmp(type, "dir") == 0 && !S_ISDIR(st.st_mode)) match_type = 0;
                            if (strcmp(type, "file") == 0 && !S_ISREG(st.st_mode)) match_type = 0;
                        }
                        if (match_type) {
                            fossil_io_printf("{cyan}Found '%s' at: %s{reset}\n", name, path);
                            closedir(dir);
                            return;
                        }
                    }
                }
            }
            closedir(dir);
            fossil_io_printf("{yellow}No match for '%s' with type '%s' in current directory.{reset}\n", name, type ? type : "(any)");
        } else {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening current directory: %s{reset}\n", strerror(errno));
        }
    } else {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error getting current working directory: %s{reset}\n", strerror(errno));
    }
}

void handle_edit(const char *file, const char *editor, int backup) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Opening file '%s' for editing...{reset}\n", file);
        fossil_io_printf("{cyan}Editor: '%s', Backup: %d{reset}\n", editor ? editor : "(default)", backup);
    }

    // If backup is requested, create a backup copy before editing
    if (backup) {
        char backup_file[1024];
        snprintf(backup_file, sizeof(backup_file), "%s.bak", file);
        if (fossil_fstream_copy(file, backup_file) == 0) {
            fossil_io_printf("{cyan}Backup created: %s{reset}\n", backup_file);
        } else {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Failed to create backup: %s{reset}\n", backup_file);
        }
    }

    // Choose editor: use provided editor or fallback to default
    const char *edit_cmd = editor;
#if defined(_WIN32) || defined(_WIN64)
    if (!edit_cmd || strlen(edit_cmd) == 0) edit_cmd = "notepad";
#else
    if (!edit_cmd || strlen(edit_cmd) == 0) edit_cmd = "nano";
#endif

    char command[1024];
    snprintf(command, sizeof(command), "%s \"%s\"", edit_cmd, file);
    system(command);

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished editing file '%s'.{reset}\n", file);
    }
}
