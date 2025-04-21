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
#include <string.h>
#include <dirent.h>

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#include <sys/statvfs.h>
#endif


// Handler function definitions
void handle_help(void) {
    fossil_io_printf("{blue,bold}Usage: shark <command> [options]{reset}\n");
    fossil_io_printf("{blue,bold}Flags:{reset}\n");
    fossil_io_printf("{cyan}  --help              Show usage and available subcommands{reset}\n");
    fossil_io_printf("{cyan}  --name              Show the name of the application{reset}\n");
    fossil_io_printf("{cyan}  --version           Prints the current version number{reset}\n");
    fossil_io_printf("{blue,bold}Commands:{reset}\n");
    fossil_io_printf("{cyan}  move                Move or rename files and directories{reset}\n");
    fossil_io_printf("{cyan}  copy                Copy files or directories{reset}\n");
    fossil_io_printf("{cyan}  delete              Delete files or directories{reset}\n");
    fossil_io_printf("{cyan}  list                List files and directories{reset}\n");
    fossil_io_printf("{cyan}  show                Display contents of a file{reset}\n");
    fossil_io_printf("{cyan}  find                Find files matching specific criteria{reset}\n");
    fossil_io_printf("{cyan}  search              Search file contents for patterns{reset}\n");
    fossil_io_printf("{cyan}  size                Display size of files or directories{reset}\n");
    fossil_io_printf("{cyan}  disk                Display disk usage and free space{reset}\n");
    fossil_io_printf("{cyan}  tree                Display directory structure as a tree{reset}\n");
    fossil_io_printf("{cyan}  compare             Compare two files or directories{reset}\n");
    fossil_io_printf("{cyan}  create              Create a new file or directory{reset}\n");
}

void handle_version(void) {
    fossil_io_printf("crabctl version %s\n", FOSSIL_APP_VERSION);
}

void handle_name(void) {
    fossil_io_printf("Application name: %s\n", FOSSIL_APP_NAME);
}

void handle_move(const char *source, const char *destination) {
    fossil_io_printf("{cyan}Moving from '%s' to '%s'...{reset}\n", source, destination);
    if (fossil_fstream_rename(source, destination) != 0) {
        perror("Error moving file");
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error moving file: %s{reset}\n", source);
    } else {
        fossil_io_printf("{cyan}Moved '%s' to '%s' successfully.{reset}\n", source, destination);
    }
}

void handle_copy(const char *source, const char *destination) {
    fossil_io_printf("{cyan}Copying from '%s' to '%s'...{reset}\n", source, destination);
    if (fossil_fstream_copy(source, destination) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error copying file: %s{reset}\n", source);
    } else {
        fossil_io_printf("{cyan}Copied '%s' to '%s' successfully.{reset}\n", source, destination);
    }
}

void handle_delete(const char *target) {
    fossil_io_printf("{cyan}Deleting '%s'...\n{reset}", target);
    if (fossil_fstream_delete(target) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error deleting file: %s{reset}\n", target);
    } else {
        fossil_io_printf("{cyan}Deleted '%s' successfully.{reset}\n", target);
    }
}

int custom_list(const char *directory) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(directory);
    if (dir == NULL) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening directory: %s{reset}\n", directory);
        return -1; // Return non-zero on failure
    }

    fossil_io_printf("{cyan}Contents of '%s':{reset}\n", directory);
    while ((entry = readdir(dir)) != NULL) {
#ifdef _WIN32
        // On Windows, skip "." and ".." explicitly
        if (fossil_io_cstring_compare(entry->d_name, ".") == 0 || fossil_io_cstring_compare(entry->d_name, "..") == 0) {
            continue;
        }
#endif
        fossil_io_printf("{green}  %s{reset}\n", entry->d_name);
    }

    closedir(dir);
    return 0; // Return 0 on success
}

void handle_list(const char *directory) {
    fossil_io_printf("{cyan}Listing contents of directory '%s'...{reset}\n", directory);
    if (custom_list(directory) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error listing directory: %s{reset}\n", directory);
    } else {
        fossil_io_printf("{cyan}Listed contents of '%s' successfully.{reset}\n", directory);
    }
}

void handle_show(const char *file) {
    fossil_io_printf("{cyan}Displaying contents of file '%s'...{reset}\n", file);
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, file, "r") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening file: %s{reset}\n", file);
        return;
    }

    char line[256];
    while (fossil_io_gets_from_stream(line, sizeof(line), stream.file) != NULL) {
        fossil_io_printf("%s", line);
    }
    fossil_fstream_close(&stream);
}

void handle_find(const char *directory, const char *pattern) {
    fossil_io_printf("{cyan}Finding files in '%s' matching pattern '%s'...{reset}\n", directory, pattern);

    DIR *dir;
    struct dirent *entry;

    dir = opendir(directory);
    if (dir == NULL) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening directory: %s{reset}\n", directory);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (fossil_io_cstring_compare(entry->d_name, ".") == 0 || fossil_io_cstring_compare(entry->d_name, "..") == 0) {
            continue;
        }

        // Check if the file name matches the pattern
        if (fossil_io_cstring_contains(entry->d_name, pattern)) {
            fossil_io_printf("{green}  %s{reset}\n", entry->d_name);
        }
    }

    closedir(dir);
    fossil_io_printf("{cyan}Finished searching in '%s'.{reset}\n", directory);
}

void handle_search(const char *file, const char *pattern) {
    fossil_io_printf("{cyan}Searching file '%s' for pattern '%s'...{reset}\n", file, pattern);
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, file, "r") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening file: %s{reset}\n", file);
        return;
    }

    char line[256];
    int line_number = 0;
    while (fossil_io_gets_from_stream(line, sizeof(line), stream.file) != NULL) {
        line_number++;
        if (fossil_io_cstring_contains(line, pattern)) {
            fossil_io_printf("{green}Line %d: %s{reset}\n", line_number, line);
        }
    }
    fossil_fstream_close(&stream);
}

void handle_size(const char *target) {
    fossil_io_printf("{cyan}Calculating size of '%s'...{reset}\n", target);
    struct stat st;
    if (stat(target, &st) == 0) {
#if defined(_WIN32) || defined(_WIN64)
        fossil_io_printf("{green}Size: %lld bytes{reset}\n", (long long)st.st_size);
#else
        fossil_io_printf("{green}Size: %ld bytes{reset}\n", st.st_size);
#endif
    } else {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error getting size: %s{reset}\n", target);
    }
}

void handle_disk(const char *path) {
    fossil_io_printf("{cyan}Calculating disk usage for '%s'...\n{reset}", path);
    struct statvfs stat;
    if (statvfs(path, &stat) == 0) {
        unsigned long total = stat.f_frsize * stat.f_blocks;
        unsigned long free = stat.f_frsize * stat.f_bfree;
        unsigned long used = total - free;
        fossil_io_printf("Total: %lu bytes\nFree: %lu bytes\nUsed: %lu bytes\n", total, free, used);
    } else {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error getting disk usage: %s{reset}\n", path);
    }
}

void handle_tree(const char *directory) {
    fossil_io_printf("{cyan}Walking directory tree for '%s'...{reset}\n", directory);
    DIR *dir;
    struct dirent *entry;

    dir = opendir(directory);
    if (dir == NULL) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening directory: %s{reset}\n", directory);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (fossil_io_cstring_compare(entry->d_name, ".") == 0 || fossil_io_cstring_compare(entry->d_name, "..") == 0) {
            continue;
        }
        fossil_io_printf("{green}  %s{reset}\n", entry->d_name);
    }

    closedir(dir);
}

void handle_compare(const char *path1, const char *path2) {
    fossil_io_printf("{cyan}Comparing '%s' with '%s'...{reset}\n", path1, path2);

    FILE *file1 = fopen(path1, "rb");
    FILE *file2 = fopen(path2, "rb");

    if (!file1 || !file2) {
        if (file1) fclose(file1);
        if (file2) fclose(file2);
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error opening files: %s or %s{reset}\n", path1, path2);
        return;
    }

    int ch1, ch2;
    int position = 0;
    while ((ch1 = fgetc(file1)) != EOF && (ch2 = fgetc(file2)) != EOF) {
        position++;
        if (ch1 != ch2) {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Files differ at byte %d: %s and %s{reset}\n", position, path1, path2);
            fclose(file1);
            fclose(file2);
            return;
        }
    }

    if (fgetc(file1) != EOF || fgetc(file2) != EOF) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Files differ in size: %s and %s{reset}\n", path1, path2);
    } else {
        fossil_io_printf("{green}Files are identical.{reset}\n");
    }

    fclose(file1);
    fclose(file2);
}

void handle_create(const char *target) {
    fossil_io_printf("{cyan}Creating '%s'...{reset}\n", target);
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, target, "w") != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error creating file: %s{reset}\n", target);
        return;
    }
    fossil_fstream_close(&stream);
    fossil_io_printf("{green}Created '%s' successfully.{reset}\n", target);
}
