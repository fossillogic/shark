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
#include "fossil/code/app.h"
#include <unistd.h>

void show_commands(char* app_name) {
    fossil_io_printf("{blue}Usage: {cyan}%s{blue} <options>{reset}\n", app_name);
    fossil_io_printf("{cyan}  --help           Display help information for the Shark tool.{reset}\n");
    fossil_io_printf("{cyan}  --version        Display the current version of the Shark tool.{reset}\n");
    fossil_io_printf("{cyan}  --name           Display app name.{reset}\n");
    fossil_io_printf("{cyan}  --verbose        Enable verbose output for all commands.{reset}\n");
    fossil_io_printf("{cyan}  --color          Set color output mode: {yellow}enable{cyan}, {yellow}disable{cyan}, or {yellow}auto{cyan}.{reset}\n");
    fossil_io_printf("{cyan}  --clear          Clear terminal or console output.{reset}\n");
    fossil_io_printf("{cyan}  create           --type=<file/dir>, --permissions=<rwx>                                Create new files or directories with specific type and permissions.{reset}\n");
    fossil_io_printf("{cyan}  delete           --recursive, --force, --trash                                       Delete files or directories. Optionally move to trash instead of hard delete.{reset}\n");
    fossil_io_printf("{cyan}  move             --force, --backup, --overwrite                                      Move or rename files and directories with overwrite and backup options.{reset}\n");
    fossil_io_printf("{cyan}  rename           --force, --backup                                                     Rename files or directories with conflict resolution.{reset}\n");
    fossil_io_printf("{cyan}  copy             --recursive, --preserve, --symlinks, --hard                       Copy files or directories with control over attributes and link types.{reset}\n");
    fossil_io_printf("{cyan}  list             --what=<files/devices/modules/hardware>, --sort, --format           List system information. Defaults to file listing. Supports devices, kernel modules, and hardware info.{reset}\n");
    fossil_io_printf("{cyan}  show             --lines=<n>, --offset=<n>, --tail, --head                         Show file contents with pagination and range options.{reset}\n");
    fossil_io_printf("{cyan}  find             --name=<pattern>, --size=<gt/lt>:<value>, --type                    Find items by name, size, or type.{reset}\n");
    fossil_io_printf("{cyan}  where            --name=<pattern>, --path=<regex>                                      Locate files or directories by name or full path pattern.{reset}\n");
    fossil_io_printf("{cyan}  search           --pattern=<regex>, --ignore-case, --whole-word                      Search inside files with advanced pattern matching.{reset}\n");
    fossil_io_printf("{cyan}  backup           --destination=<path>, --compress, --encrypt                         Backup files or directories with optional compression and encryption.{reset}\n");
    fossil_io_printf("{cyan}  size             --human-readable, --total, --summarize                              Display size of items with formatting and summary options.{reset}\n");
    fossil_io_printf("{cyan}  disk             --all, --free, --used, --inodes                                   Show disk statistics including inode usage.{reset}\n");
    fossil_io_printf("{cyan}  tree             --depth=<n>, --all, --dirs-only, --files-only                     Print directory tree with control over depth and content.{reset}\n");
    fossil_io_printf("{cyan}  compare          --ignore-case, --binary, --diff, --hash                           Compare files or directories using various strategies.{reset}\n");
    fossil_io_printf("{cyan}  info             --details, --type, --stat, --checksum                             Display metadata and statistics about a file or directory.{reset}\n");
    fossil_io_printf("{cyan}  clean            --temp, --cache, --logs, --dry-run                                Clean up generated or temporary files, with optional preview.{reset}\n");
    fossil_io_printf("{cyan}  file             --create, --modify, --delete, --split=<lines/bytes>, --join=<file1,file2,...>, --output=<file> Perform basic file operations. Includes splitting and joining files.{reset}\n");
    fossil_io_printf("{cyan}  ask              --exists=<path>, --not-exist, --type=<file/dir>                     Check for file/directory existence or type.{reset}\n");
    fossil_io_printf("{cyan}  change           --target=<path>, --value=<new>, --owner, --mode                   Modify file properties like permissions or ownership.{reset}\n");
    fossil_io_printf("{cyan}  diff             --unified, --side-by-side, --ignore-case, --context=<n>           Show textual differences between files. Supports unified and side-by-side views.{reset}\n");
    fossil_io_printf("{cyan}  archive          --create, --extract, --format=zip/tar/gz, --output=<file>         Create or extract archive files in supported formats.{reset}\n");
    exit(FOSSIL_IO_SUCCESS);
}

void show_version(void) {
    fossil_io_printf("{blue}%s version %s{reset}\n", FOSSIL_APP_NAME, FOSSIL_APP_VERSION);
    exit(FOSSIL_IO_SUCCESS);
}

bool app_entry(int argc, char** argv) {
    for (i32 i = 1; i < argc; ++i) {
        if (argv[i] == cnullptr) continue;

        if (fossil_io_cstring_compare(argv[i], "--help") == 0) {
            show_commands(argv[0]);
        } else if (fossil_io_cstring_compare(argv[i], "--version") == 0) {
            show_version();
        } else if (fossil_io_cstring_compare(argv[i], "--name") == 0) {
            fossil_io_printf("{blue}App Name: {cyan}%s{reset}\n", FOSSIL_APP_NAME);
        } else if (fossil_io_cstring_compare(argv[i], "--verbose") == 0) {
            FOSSIL_IO_VERBOSE = true;
            fossil_io_printf("{blue}Verbose mode enabled{reset}\n");
        } else if (fossil_io_cstring_compare(argv[i], "--color") == 0) {
            if (i + 1 < argc && argv[i + 1] != cnullptr) {
                if (fossil_io_cstring_compare(argv[i + 1], "enable") == 0) {
                    FOSSIL_IO_COLOR_ENABLE = true;
                } else if (fossil_io_cstring_compare(argv[i + 1], "disable") == 0) {
                    FOSSIL_IO_COLOR_ENABLE = false;
                } else if (fossil_io_cstring_compare(argv[i + 1], "auto") == 0) {
                    FOSSIL_IO_COLOR_ENABLE = isatty(STDOUT_FILENO);
                }
                ++i; // Skip next argument
            }
        } else if (fossil_io_cstring_compare(argv[i], "--clear") == 0) {
            fossil_io_printf("\033[H\033[J"); // ANSI escape sequence to clear screen
        }
    }
    return 0;
}
