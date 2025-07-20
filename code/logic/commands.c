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

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#include <windows.h> // Include for ULARGE_INTEGER and GetDiskFreeSpaceEx
#else
#include <sys/statvfs.h>
#endif

int ENABLE_VERBOSE = 0; // Global variable to control verbose output


// Handler function definitions
void handle_help(void) {
    const char *flags[] = {
        "--help              Show usage and available subcommands",
        "--name              Show the name of the application",
        "--version           Prints the current version number",
        "--verbose           Enable verbose output",
        "--clear             Clear the console screen"
    };

    const char *commands[] = {
        "move                Move or rename files and directories",
        "rename              Rename files or directories",
        "copy                Copy files or directories",
        "delete              Delete files or directories",
        "list                List files and directories",
        "show                Display contents of a file",
        "find                Find files matching specific criteria",
        "where               Locate files or directories by name",
        "search              Search file contents for patterns",
        "ask                 Prompt for user input",
        "edit                Edit files interactively",
        "size                Display size of files or directories",
        "disk                Display disk usage and free space",
        "tree                Display directory structure as a tree",
        "compare             Compare two files or directories",
        "create              Create a new file or directory",
        "color=<enable|disable|auto> Set color output mode"
    };

    fossil_io_printf("{blue,bold}Usage: shark <command> [options]{reset}\n");
    fossil_io_printf("{blue,bold}Flags:{reset}\n");
    for (size_t i = 0; i < sizeof(flags) / sizeof(flags[0]); i++) {
        fossil_io_printf("{cyan} > %s{reset}\n", flags[i]);
    }

    fossil_io_printf("{blue,bold}Commands:{reset}\n");
    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        fossil_io_printf("{cyan} > %s{reset}\n", commands[i]);
    }
}

void handle_version(void) {
    fossil_io_printf("shark version %s\n", FOSSIL_APP_VERSION);
}

void handle_name(void) {
    fossil_io_printf("Application name: %s\n", FOSSIL_APP_NAME);
}

void handle_move(const char *source, const char *destination) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Moving from '%s' to '%s'...{reset}\n", source, destination);
    }
    if (fossil_fstream_rename(source, destination) != 0) {
        perror("Error moving file");
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error moving file: %s{reset}\n", source);
    } else {
        if (ENABLE_VERBOSE) {
            fossil_io_printf("{cyan}Moved '%s' to '%s' successfully.{reset}\n", source, destination);
        }
    }
}

void handle_copy(const char *source, const char *destination) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Copying from '%s' to '%s'...{reset}\n", source, destination);
    }
    if (fossil_fstream_copy(source, destination) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error copying file: %s{reset}\n", source);
    } else {
        if (ENABLE_VERBOSE) {
            fossil_io_printf("{cyan}Copied '%s' to '%s' successfully.{reset}\n", source, destination);
        }
    }
}

void handle_delete(const char *target) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Deleting '%s'...\n{reset}", target);
    }
    if (fossil_fstream_delete(target) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error deleting file: %s{reset}\n", target);
    } else {
        if (ENABLE_VERBOSE) {
            fossil_io_printf("{cyan}Deleted '%s' successfully.{reset}\n", target);
        }
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

void handle_list(const char *directory) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Listing contents of directory '%s'...{reset}\n", directory);
    }
    if (custom_list(directory) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error listing directory: %s{reset}\n", directory);
    } else {
        if (ENABLE_VERBOSE) {
            fossil_io_printf("{cyan}Listed contents of '%s' successfully.{reset}\n", directory);
        }
    }
}

void handle_show(const char *file) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Displaying contents of file '%s'...{reset}\n", file);
    }
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, file, "r") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening file: %s{reset}\n", file);
        return;
    }

    int ch;
    while ((ch = fossil_io_getc(&stream)) != EOF) {
        fossil_io_printf("%c", ch);
    }
    fossil_fstream_close(&stream);
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished displaying contents of file '%s'.{reset}\n", file);
    }
}

void handle_find(const char *directory, const char *pattern) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finding files in '%s' matching pattern '%s'...{reset}\n", directory, pattern);
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

        // Check if the file name matches the pattern
        if (fossil_io_cstring_contains(entry->d_name, pattern)) {
            fossil_io_printf("{cyan}  %s{reset}\n", entry->d_name);
        }
    }

    closedir(dir);
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished searching in '%s'.{reset}\n", directory);
    }
}

void handle_search(const char *file, const char *pattern) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Searching file '%s' for pattern '%s'...{reset}\n", file, pattern);
    }

    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, file, "r") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening file: %s{reset}\n", file);
        return;
    }

    char line[256];
    int line_number = 0;
    while (fossil_io_gets_from_stream(line, sizeof(line), &stream) != cnull) {
        line_number++;
        if (fossil_io_cstring_contains(line, pattern)) {
            fossil_io_printf("{cyan}Line %d: %s{reset}\n", line_number, line);
        }
    }
    fossil_fstream_close(&stream);

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished searching file '%s'.{reset}\n", file);
    }
}

void handle_size(const char *target) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{blue,bold}Calculating size of '%s'...{reset}\n", target);
    }

    struct stat st;
    if (stat(target, &st) == 0) {
        long long size_bytes = (long long)st.st_size;
        double size_kb = size_bytes / 1024.0;
        double size_mb = size_kb / 1024.0;
        double size_gb = size_mb / 1024.0;

        fossil_io_printf("{blue,bold}Size of '%s':{reset}\n", target);
        fossil_io_printf("{cyan}  %lld bytes{reset}\n", size_bytes);
        fossil_io_printf("{cyan}  %.2f KB{reset}\n", size_kb);
        fossil_io_printf("{cyan}  %.2f MB{reset}\n", size_mb);
        fossil_io_printf("{cyan}  %.2f GB{reset}\n", size_gb);
    } else {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error getting size of '%s': %s{reset}\n", target, strerror(errno));
    }
    cunused(target); // To avoid unused parameter warning

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{blue,bold}Finished calculating size of '%s'.{reset}\n", target);
    }
}

void handle_disk(const char *path) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Calculating disk usage for '%s'...{reset}\n", path);
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
        fossil_io_printf("{cyan}Disk Usage for '%s':{reset}\n", path);
        fossil_io_printf("  Total: %llu bytes\n  Free: %llu bytes\n  Used: %llu bytes\n", total, free, used);
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
        fossil_io_printf("{cyan}Disk Usage for '%s':{reset}\n", path);
        fossil_io_printf("  Total: %llu bytes\n  Free: %llu bytes\n  Available: %llu bytes\n  Used: %llu bytes\n", total, free, available, used);
    } else {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error getting disk usage: %s{reset}\n", strerror(errno));
    }
#endif

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished calculating disk usage for '%s'.{reset}\n", path);
    }
}

void handle_tree(const char *directory) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Walking directory tree for '%s' (one level)...{reset}\n", directory);
    }

    DIR *dir;
    struct dirent *entry;

    dir = opendir(directory);
    if (dir == cnull) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening directory: %s{reset}\n", directory);
        return;
    }

    fossil_io_printf("{cyan}%s{reset}\n", directory);

    while ((entry = readdir(dir)) != cnull) {
        // Skip "." and ".."
        if (fossil_io_cstring_compare(entry->d_name, ".") == 0 || fossil_io_cstring_compare(entry->d_name, "..") == 0) {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);

        struct stat st;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            fossil_io_printf("{cyan}├── [DIR]  %s{reset}\n", entry->d_name);
        } else {
            fossil_io_printf("{cyan}├──       %s{reset}\n", entry->d_name);
        }
    }

    closedir(dir);
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished walking directory tree for '%s'.{reset}\n", directory);
    }
}

void handle_compare(const char *path1, const char *path2) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Comparing '%s' with '%s'...{reset}\n", path1, path2);
    }

    fossil_fstream_t stream1, stream2;
    if (fossil_fstream_open(&stream1, path1, "r") != 0 || fossil_fstream_open(&stream2, path2, "r") != 0) {
        if (fossil_fstream_is_open(&stream1)) fossil_fstream_close(&stream1);
        if (fossil_fstream_is_open(&stream2)) fossil_fstream_close(&stream2);
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening files: %s or %s{reset}\n", path1, path2);
        return;
    }

    char buf1[256], buf2[256];
    int line_number = 0;
    while (fossil_io_gets_from_stream(buf1, sizeof(buf1), &stream1) != cnull &&
           fossil_io_gets_from_stream(buf2, sizeof(buf2), &stream2) != cnull) {
        line_number++;
        if (strcmp(buf1, buf2) != 0) {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Files differ at line %d: %s and %s{reset}\n", line_number, path1, path2);
            fossil_fstream_close(&stream1);
            fossil_fstream_close(&stream2);
            return;
        }
    }

    if (fossil_io_gets_from_stream(buf1, sizeof(buf1), &stream1) != cnull ||
        fossil_io_gets_from_stream(buf2, sizeof(buf2), &stream2) != cnull) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Files differ in size: %s and %s{reset}\n", path1, path2);
    } else {
        fossil_io_printf("{cyan}Files are identical.{reset}\n");
    }

    fossil_fstream_close(&stream1);
    fossil_fstream_close(&stream2);

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished comparing '%s' with '%s'.{reset}\n", path1, path2);
    }
}

void handle_create(const char *target) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Creating '%s'...{reset}\n", target);
    }
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, target, "w") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error creating file: %s{reset}\n", target);
        return;
    }
    fossil_fstream_close(&stream);
    fossil_io_printf("{cyan}Created '%s' successfully.{reset}\n", target);

    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished creating '%s'.{reset}\n", target);
    }
}

const char *handle_ask(const char *prompt) {
    static char response[256];
    fossil_io_printf("%s ", prompt);
    if (fossil_io_gets(response, sizeof(response)) == cnull) {
        return cnull;
    }
    // Check if file exists
    struct stat st;
    if (stat(response, &st) == 0) {
        fossil_io_printf("{green}File '%s' exists.{reset}\n", response);
    } else {
        fossil_io_printf("{red}File '%s' does not exist.{reset}\n", response);
    }
    return response;
}

void handle_rename(const char *old_name, const char *new_name) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Renaming '%s' to '%s'...{reset}\n", old_name, new_name);
    }
    if (fossil_fstream_rename(old_name, new_name) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error renaming file: %s to %s{reset}\n", old_name, new_name);
    } else {
        if (ENABLE_VERBOSE) {
            fossil_io_printf("{cyan}Renamed '%s' to '%s' successfully.{reset}\n", old_name, new_name);
        }
    }
}

void handle_where(void) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != cnull) {
        fossil_io_printf("{cyan}Current working directory: %s{reset}\n", cwd);
    } else {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error getting current working directory: %s{reset}\n", strerror(errno));
    }
}

void handle_edit(const char *file) {
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Opening file '%s' for editing...{reset}\n", file);
    }
#if defined(_WIN32) || defined(_WIN64)
    // On Windows, try to open with notepad
    char command[1024];
    snprintf(command, sizeof(command), "notepad \"%s\"", file);
    system(command);
#else
    // On Unix-like systems, use nano editor
    char command[1024];
    snprintf(command, sizeof(command), "nano \"%s\"", file);
    system(command);
#endif
    if (ENABLE_VERBOSE) {
        fossil_io_printf("{cyan}Finished editing file '%s'.{reset}\n", file);
    }
}
