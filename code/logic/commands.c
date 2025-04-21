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
#include <dirent.h> // for custom_list to use opendir, readdir, closedir
#include <sys/stat.h> // for mkdir

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
    fossil_io_printf("{cyan}  meta                Show metadata of a file or directory{reset}\n");
    fossil_io_printf("{cyan}  compress            Compress files or directories{reset}\n");
    fossil_io_printf("{cyan}  extract             Extract files from archives{reset}\n");
    fossil_io_printf("{cyan}  chmod               Change file permissions{reset}\n");
    fossil_io_printf("{cyan}  ownership           Change file ownership (user/group){reset}\n");
    fossil_io_printf("{cyan}  find                Find files matching specific criteria{reset}\n");
    fossil_io_printf("{cyan}  search              Search file contents for patterns{reset}\n");
    fossil_io_printf("{cyan}  size                Display size of files or directories{reset}\n");
    fossil_io_printf("{cyan}  disk                Display disk usage and free space{reset}\n");
    fossil_io_printf("{cyan}  tree                Display directory structure as a tree{reset}\n");
    fossil_io_printf("{cyan}  compare             Compare two files or directories{reset}\n");
    fossil_io_printf("{cyan}  sync                Synchronize files between directories{reset}\n");
    fossil_io_printf("{cyan}  update              Update file timestamps{reset}\n");
    fossil_io_printf("{cyan}  open                Open file with a specified editor or viewer{reset}\n");
    fossil_io_printf("{cyan}  edit                Open file in a default or specified editor for editing{reset}\n");
    fossil_io_printf("{cyan}  push                Push files to a remote location{reset}\n");
    fossil_io_printf("{cyan}  pull                Pull files from a remote location{reset}\n");
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
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
#endif
        fossil_io_printf("{green}  %s{reset}\n", entry->d_name);
    }

    closedir(dir);
    return 0; // Return 0 on success
}

void handle_list(const char *directory) {
    fossil_io_printf("Listing contents of directory '%s'...\n", directory);
    if (custom_list(directory) != 0) {
        fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Error listing directory: %s{reset}\n", directory);
    } else {
        fossil_io_printf("{cyan}Listed contents of '%s' successfully.{reset}\n", directory);
    }
}

void handle_show(const char *file) {
    fossil_io_printf("Showing contents of file '%s'...\n", file);
}

void handle_meta(const char *target) {
    fossil_io_printf("Displaying metadata for '%s'...\n", target);
}

void handle_compress(const char *source, const char *archive) {
    fossil_io_printf("Compressing '%s' into archive '%s'...\n", source, archive);
}

void handle_extract(const char *archive, const char *destination) {
    fossil_io_printf("Extracting archive '%s' to '%s'...\n", archive, destination);
}

void handle_chmod(const char *target, const char *permissions) {
    fossil_io_printf("Changing permissions of '%s' to '%s'...\n", target, permissions);
}

void handle_ownership(const char *target, const char *owner) {
    fossil_io_printf("Changing ownership of '%s' to user '%s'...\n", target, owner);
}

void handle_find(const char *directory, const char *pattern) {
    fossil_io_printf("Finding entries in '%s' matching pattern '%s'...\n", directory, pattern);
}

void handle_search(const char *file, const char *pattern) {
    fossil_io_printf("Searching file '%s' for pattern '%s'...\n", file, pattern);
}

void handle_size(const char *target) {
    fossil_io_printf("Calculating size of '%s'...\n", target);
}

void handle_disk(const char *path) {
    fossil_io_printf("Displaying disk usage for path '%s'...\n", path);
}

void handle_tree(const char *directory) {
    fossil_io_printf("Displaying directory tree for '%s'...\n", directory);
}

void handle_compare(const char *path1, const char *path2) {
    fossil_io_printf("Comparing '%s' with '%s'...\n", path1, path2);
}

void handle_sync(const char *source, const char *destination) {
    fossil_io_printf("Synchronizing from '%s' to '%s'...\n", source, destination);
}

void handle_update(const char *target) {
    fossil_io_printf("Updating '%s'...\n", target);
}

void handle_open(const char *file) {
    fossil_io_printf("Opening file '%s'...\n", file);
}

void handle_edit(const char *file) {
    fossil_io_printf("Editing file '%s'...\n", file);
}

void handle_push(const char *source, const char *remote) {
    fossil_io_printf("Pushing from '%s' to remote '%s'...\n", source, remote);
}

void handle_pull(const char *remote, const char *destination) {
    fossil_io_printf("Pulling from remote '%s' to '%s'...\n", remote, destination);
}
