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

int FOSSIL_IO_VERBOSE = false; // Verbose output flag

void show_commands(char* app_name) {
    fossil_io_printf("{blue}Usage: {cyan}%s{blue} <command> [options]{reset}\n", app_name);
    fossil_io_printf("{blue}Options:{reset}\n");
    fossil_io_printf("{cyan}  --help           {reset}| Display help information for the Shark tool.\n");
    fossil_io_printf("{cyan}  --version        {reset}| Display the current version of the Shark tool.\n");
    fossil_io_printf("{cyan}  --name           {reset}| Display app name.\n");
    fossil_io_printf("{cyan}  --verbose        {reset}| Enable verbose output for all commands.\n");
    fossil_io_printf("{cyan}  --color          {reset}| Set color output mode: {yellow}enable{cyan}, {yellow}disable{cyan}, or {yellow}auto{cyan}.\n");
    fossil_io_printf("{cyan}  --clear          {reset}| Clear terminal or console output.\n");
    fossil_io_printf("{blue}Commands:{reset}\n");
    fossil_io_printf("{cyan}  create            {reset}| {yellow}--type=<file/dir>, --name=<file_name>{reset}                  | Create new files or directories with specified type and name.\n");
    fossil_io_printf("{cyan}  delete            {reset}| {yellow}--force{reset}                                                | Remove files or directories. Optionally move them to trash instead of permanent deletion.\n");
    fossil_io_printf("{cyan}  move              {reset}| {yellow}--force, --backup{reset}                                      | Move or rename files and directories with options to overwrite or create backups.\n");
    fossil_io_printf("{cyan}  rename            {reset}| {yellow}--force, --backup{reset}                                      | Rename files or directories with conflict handling options.\n");
    fossil_io_printf("{cyan}  copy              {reset}| {yellow}--preserve, --link=<hard/sym>{reset}                          | Duplicate files or directories while preserving attributes or creating links.\n");
    fossil_io_printf("{cyan}  list              {reset}| {yellow}--as=<list/tree>, --sort=<disc/asc>{reset}                    | Display files or directories in a list or tree format with sorting options.\n");
    fossil_io_printf("{cyan}  show              {reset}| {yellow}--lines=<n>, --offset=<n>{reset}                              | View file contents with support for pagination and line offsets.\n");
    fossil_io_printf("{cyan}  find              {reset}| {yellow}--name=<pattern>, --type{reset}                               | Search for files or directories by name, type, or other criteria.\n");
    fossil_io_printf("{cyan}  size              {reset}| {yellow}--human-readable{reset}                                       | Show the size of files or directories with optional human-readable formatting.\n");
    fossil_io_printf("{cyan}  compare           {reset}| {yellow}--binary, --diff, --hash{reset}                               | Compare files or directories using binary, diff, or hash methods.\n");
    fossil_io_printf("{cyan}  ask               {reset}| {yellow}--exists=<path>, --not-exist=<path>, --type=<file/dir>{reset} | Verify the existence or type of files or directories.\n");
    fossil_io_printf("{cyan}  change            {reset}| {yellow}--target=<path>, --value=<new>, --owner, --mode{reset}        | Update file properties such as permissions, ownership, or mode.\n");
    
    exit(FOSSIL_IO_SUCCESS);
}

void show_version(void) {
    fossil_io_printf("{blue}%s version %s{reset}\n", FOSSIL_APP_NAME, FOSSIL_APP_VERSION);
    exit(FOSSIL_IO_SUCCESS);
}

void show_name(void) {
    fossil_io_printf("{blue}App Name: {cyan}%s{reset}\n", FOSSIL_APP_NAME);
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
            show_name();
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
        } else if (fossil_io_cstring_compare(argv[i], "create") == 0) {
            const char *type = cnullptr, *name = cnullptr;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_starts_with(argv[i], "--type=")) {
                    type = argv[i] + 7;
                } else if (fossil_io_cstring_starts_with(argv[i], "--name=")) {
                    name = argv[i] + 7;
                } else {
                    break;
                }
            }
            shark_create(argv[0], type, name);
        } else if (fossil_io_cstring_compare(argv[i], "delete") == 0) {
            bool force = false;
            const char *file = cnullptr;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--force") == 0) {
                    force = true;
                } else {
                    file = argv[i];
                }
            }
            shark_delete(file, force);
        } else if (fossil_io_cstring_compare(argv[i], "move") == 0) {
            const char *source = cnullptr, *destination = cnullptr;
            bool force = false, backup = false;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--force") == 0) {
                    force = true;
                } else if (fossil_io_cstring_compare(argv[i], "--backup") == 0) {
                    backup = true;
                } else if (!source) {
                    source = argv[i];
                } else {
                    destination = argv[i];
                }
            }
            shark_move(source, destination, force, backup);
        } else if (fossil_io_cstring_compare(argv[i], "rename") == 0) {
            const char *old_name = cnullptr, *new_name = cnullptr;
            bool force = false, backup = false;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--force") == 0) {
                    force = true;
                } else if (fossil_io_cstring_compare(argv[i], "--backup") == 0) {
                    backup = true;
                } else if (!old_name) {
                    old_name = argv[i];
                } else {
                    new_name = argv[i];
                }
            }
            shark_rename(old_name, new_name, force, backup);
        } else if (fossil_io_cstring_compare(argv[i], "copy") == 0) {
            const char *source = cnullptr, *destination = cnullptr, *file_link = cnullptr;
            bool preserve = false;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--preserve") == 0) {
                    preserve = true;
                } else if (fossil_io_cstring_starts_with(argv[i], "--link=")) {
                    file_link = argv[i] + 7;
                } else if (!source) {
                    source = argv[i];
                } else {
                    destination = argv[i];
                }
            }
            shark_copy(source, destination, preserve, file_link);
        } else if (fossil_io_cstring_compare(argv[i], "list") == 0) {
            const char *path = cnullptr, *as = cnullptr, *sort = cnullptr;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_starts_with(argv[i], "--as=")) {
                    as = argv[i] + 5;
                } else if (fossil_io_cstring_starts_with(argv[i], "--sort=")) {
                    sort = argv[i] + 7;
                } else {
                    path = argv[i];
                }
            }
            shark_list(path, as, sort);
        } else if (fossil_io_cstring_compare(argv[i], "show") == 0) {
            const char *file = cnullptr;
            int lines = 0, offset = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_starts_with(argv[i], "--lines=")) {
                    lines = atoi(argv[i] + 8);
                } else if (fossil_io_cstring_starts_with(argv[i], "--offset=")) {
                    offset = atoi(argv[i] + 9);
                } else {
                    file = argv[i];
                }
            }
            shark_show(file, lines, offset);
        } else if (fossil_io_cstring_compare(argv[i], "find") == 0) {
            const char *path = cnullptr, *name = cnullptr, *type = cnullptr;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_starts_with(argv[i], "--name=")) {
                    name = argv[i] + 7;
                } else if (fossil_io_cstring_starts_with(argv[i], "--type=")) {
                    type = argv[i] + 7;
                } else {
                    path = argv[i];
                }
            }
            shark_find(path, name, type);
        } else if (fossil_io_cstring_compare(argv[i], "size") == 0) {
            const char *path = cnullptr;
            bool human_readable = false;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--human-readable") == 0) {
                    human_readable = true;
                } else {
                    path = argv[i];
                }
            }
            shark_size(path, human_readable);
        } else if (fossil_io_cstring_compare(argv[i], "compare") == 0) {
            const char *path1 = cnullptr, *path2 = cnullptr;
            bool binary = false, diff = false, hash = false;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--binary") == 0) {
                    binary = true;
                } else if (fossil_io_cstring_compare(argv[i], "--diff") == 0) {
                    diff = true;
                } else if (fossil_io_cstring_compare(argv[i], "--hash") == 0) {
                    hash = true;
                } else if (!path1) {
                    path1 = argv[i];
                } else {
                    path2 = argv[i];
                }
            }
            shark_compare(path1, path2, binary, diff, hash);
        } else if (fossil_io_cstring_compare(argv[i], "ask") == 0) {
            const char *exists = cnullptr, *not_exist = cnullptr, *type = cnullptr;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_starts_with(argv[i], "--exists=")) {
                    exists = argv[i] + 9;
                } else if (fossil_io_cstring_starts_with(argv[i], "--not-exist=")) {
                    not_exist = argv[i] + 13;
                } else if (fossil_io_cstring_starts_with(argv[i], "--type=")) {
                    type = argv[i] + 7;
                }
            }
            shark_ask(exists, not_exist, type);
        } else if (fossil_io_cstring_compare(argv[i], "change") == 0) {
            const char *target = cnullptr, *value = cnullptr;
            bool owner = false, mode = false;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_starts_with(argv[i], "--target=")) {
                    target = argv[i] + 9;
                } else if (fossil_io_cstring_starts_with(argv[i], "--value=")) {
                    value = argv[i] + 8;
                } else if (fossil_io_cstring_compare(argv[i], "--owner") == 0) {
                    owner = true;
                } else if (fossil_io_cstring_compare(argv[i], "--mode") == 0) {
                    mode = true;
                }
            }
            shark_change(target, value, owner, mode);
        } else {
            fossil_io_printf("{red}Unknown command: %s{reset}\n", argv[i]);
            return false;
        }
    }
    return 0;
}
