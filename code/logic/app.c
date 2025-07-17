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
    fossil_io_printf("{blue}Options:{reset}\n");
    fossil_io_printf("{cyan}  --help           Display help information for the Shark tool.{reset}\n");
    fossil_io_printf("{cyan}  --version        Display the current version of the Shark tool.{reset}\n");
    fossil_io_printf("{cyan}  --name           Display app name.{reset}\n");
    fossil_io_printf("{cyan}  --verbose        Enable verbose output for all commands.{reset}\n");
    fossil_io_printf("{cyan}  --color          Set color output mode: {yellow}enable{cyan}, {yellow}disable{cyan}, or {yellow}auto{cyan}.{reset}\n");
    fossil_io_printf("{cyan}  --clear          Clear terminal or console output.{reset}\n");
    fossil_io_printf("{blue}Commands:{reset}\n");
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
        } else if (fossil_io_cstring_compare(argv[i], "create") == 0) {
            const char *type = cnullptr, *permissions = cnullptr;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (strncmp(argv[i], "--type=", 7) == 0) {
                    type = argv[i] + 7;
                } else if (strncmp(argv[i], "--permissions=", 14) == 0) {
                    permissions = argv[i] + 14;
                }
            }
            shark_create(type, permissions);
        } else if (fossil_io_cstring_compare(argv[i], "delete") == 0) {
            int recursive = 0, force = 0, trash = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--recursive") == 0) {
                    recursive = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--force") == 0) {
                    force = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--trash") == 0) {
                    trash = 1;
                }
            }
            shark_delete(recursive, force, trash);
        } else if (fossil_io_cstring_compare(argv[i], "move") == 0) {
            int force = 0, backup = 0, overwrite = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--force") == 0) {
                    force = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--backup") == 0) {
                    backup = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--overwrite") == 0) {
                    overwrite = 1;
                }
            }
            shark_move(force, backup, overwrite);
        } else if (fossil_io_cstring_compare(argv[i], "rename") == 0) {
            int force = 0, backup = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--force") == 0) {
                    force = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--backup") == 0) {
                    backup = 1;
                }
            }
            shark_rename(force, backup);
        } else if (fossil_io_cstring_compare(argv[i], "copy") == 0) {
            int recursive = 0, preserve = 0, symlinks = 0, hard = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--recursive") == 0) {
                    recursive = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--preserve") == 0) {
                    preserve = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--symlinks") == 0) {
                    symlinks = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--hard") == 0) {
                    hard = 1;
                }
            }
            shark_copy(recursive, preserve, symlinks, hard);
        } else if (fossil_io_cstring_compare(argv[i], "list") == 0) {
            const char *what = cnullptr, *sort = cnullptr, *format = cnullptr;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (strncmp(argv[i], "--what=", 7) == 0) {
                    what = argv[i] + 7;
                } else if (strncmp(argv[i], "--sort=", 7) == 0) {
                    sort = argv[i] + 7;
                } else if (strncmp(argv[i], "--format=", 9) == 0) {
                    format = argv[i] + 9;
                }
            }
            shark_list(what, sort, format);
        } else if (fossil_io_cstring_compare(argv[i], "show") == 0) {
            int lines = -1, offset = -1, tail = 0, head = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (strncmp(argv[i], "--lines=", 8) == 0) {
                    lines = atoi(argv[i] + 8);
                } else if (strncmp(argv[i], "--offset=", 9) == 0) {
                    offset = atoi(argv[i] + 9);
                } else if (fossil_io_cstring_compare(argv[i], "--tail") == 0) {
                    tail = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--head") == 0) {
                    head = 1;
                }
            }
            shark_show(lines, offset, tail, head);
        } else if (fossil_io_cstring_compare(argv[i], "find") == 0) {
            const char *name = cnullptr, *size = cnullptr, *type = cnullptr;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (strncmp(argv[i], "--name=", 7) == 0) {
                    name = argv[i] + 7;
                } else if (strncmp(argv[i], "--size=", 7) == 0) {
                    size = argv[i] + 7;
                } else if (strncmp(argv[i], "--type=", 7) == 0) {
                    type = argv[i] + 7;
                }
            }
            shark_find(name, size, type);
        } else if (fossil_io_cstring_compare(argv[i], "where") == 0) {
            const char *name = cnullptr, *path = cnullptr;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (strncmp(argv[i], "--name=", 7) == 0) {
                    name = argv[i] + 7;
                } else if (strncmp(argv[i], "--path=", 7) == 0) {
                    path = argv[i] + 7;
                }
            }
            shark_where(name, path);
        } else if (fossil_io_cstring_compare(argv[i], "search") == 0) {
            const char *pattern = cnullptr;
            int ignore_case = 0, whole_word = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (strncmp(argv[i], "--pattern=", 10) == 0) {
                    pattern = argv[i] + 10;
                } else if (fossil_io_cstring_compare(argv[i], "--ignore-case") == 0) {
                    ignore_case = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--whole-word") == 0) {
                    whole_word = 1;
                }
            }
            shark_search(pattern, ignore_case, whole_word);
        } else if (fossil_io_cstring_compare(argv[i], "backup") == 0) {
            const char *destination = cnullptr;
            int compress = 0, encrypt = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (strncmp(argv[i], "--destination=", 15) == 0) {
                    destination = argv[i] + 15;
                } else if (fossil_io_cstring_compare(argv[i], "--compress") == 0) {
                    compress = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--encrypt") == 0) {
                    encrypt = 1;
                }
            }
            shark_backup(destination, compress, encrypt);
        } else if (fossil_io_cstring_compare(argv[i], "size") == 0) {
            int human_readable = 0, total = 0, summarize = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--human-readable") == 0) {
                    human_readable = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--total") == 0) {
                    total = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--summarize") == 0) {
                    summarize = 1;
                }
            }
            shark_size(human_readable, total, summarize);
        } else if (fossil_io_cstring_compare(argv[i], "disk") == 0) {
            int all = 0, free = 0, used = 0, inodes = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--all") == 0) {
                    all = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--free") == 0) {
                    free = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--used") == 0) {
                    used = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--inodes") == 0) {
                    inodes = 1;
                }
            }
            shark_disk(all, free, used, inodes);
        } else if (fossil_io_cstring_compare(argv[i], "tree") == 0) {
            int depth = -1, all = 0, dirs_only = 0, files_only = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (strncmp(argv[i], "--depth=", 8) == 0) {
                    depth = atoi(argv[i] + 8);
                } else if (fossil_io_cstring_compare(argv[i], "--all") == 0) {
                    all = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--dirs-only") == 0) {
                    dirs_only = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--files-only") == 0) {
                    files_only = 1;
                }
            }
            shark_tree(depth, all, dirs_only, files_only);
        } else if (fossil_io_cstring_compare(argv[i], "compare") == 0) {
            int ignore_case = 0, binary = 0, diff = 0, hash = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--ignore-case") == 0) {
                    ignore_case = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--binary") == 0) {
                    binary = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--diff") == 0) {
                    diff = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--hash") == 0) {
                    hash = 1;
                }
            }
            shark_compare(ignore_case, binary, diff, hash);
        } else if (fossil_io_cstring_compare(argv[i], "info") == 0) {
            int details = 0, stat = 0, checksum = 0;
            const char *type = cnullptr;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--details") == 0) {
                    details = 1;
                } else if (strncmp(argv[i], "--type=", 7) == 0) {
                    type = argv[i] + 7;
                } else if (fossil_io_cstring_compare(argv[i], "--stat") == 0) {
                    stat = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--checksum") == 0) {
                    checksum = 1;
                }
            }
            shark_info(details, type, stat, checksum);
        } else if (fossil_io_cstring_compare(argv[i], "clean") == 0) {
            int temp = 0, cache = 0, logs = 0, dry_run = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--temp") == 0) {
                    temp = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--cache") == 0) {
                    cache = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--logs") == 0) {
                    logs = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--dry-run") == 0) {
                    dry_run = 1;
                }
            }
            shark_clean(temp, cache, logs, dry_run);
        } else if (fossil_io_cstring_compare(argv[i], "file") == 0) {
            int create = 0, modify = 0, delete_op = 0;
            const char *split = cnullptr, *join = cnullptr, *output = cnullptr;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--create") == 0) {
                    create = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--modify") == 0) {
                    modify = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--delete") == 0) {
                    delete_op = 1;
                } else if (strncmp(argv[i], "--split=", 8) == 0) {
                    split = argv[i] + 8;
                } else if (strncmp(argv[i], "--join=", 7) == 0) {
                    join = argv[i] + 7;
                } else if (strncmp(argv[i], "--output=", 9) == 0) {
                    output = argv[i] + 9;
                }
            }
            shark_file(create, modify, delete_op, split, join, output);
        } else if (fossil_io_cstring_compare(argv[i], "ask") == 0) {
            const char *exists = cnullptr, *type = cnullptr;
            int not_exist = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (strncmp(argv[i], "--exists=", 9) == 0) {
                    exists = argv[i] + 9;
                } else if (fossil_io_cstring_compare(argv[i], "--not-exist") == 0) {
                    not_exist = 1;
                } else if (strncmp(argv[i], "--type=", 7) == 0) {
                    type = argv[i] + 7;
                }
            }
            shark_ask(exists, not_exist, type);
        } else if (fossil_io_cstring_compare(argv[i], "change") == 0) {
            const char *target = cnullptr, *value = cnullptr;
            int owner = 0, mode = 0;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (strncmp(argv[i], "--target=", 9) == 0) {
                    target = argv[i] + 9;
                } else if (strncmp(argv[i], "--value=", 8) == 0) {
                    value = argv[i] + 8;
                } else if (fossil_io_cstring_compare(argv[i], "--owner") == 0) {
                    owner = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--mode") == 0) {
                    mode = 1;
                }
            }
            shark_change(target, value, owner, mode);
        } else if (fossil_io_cstring_compare(argv[i], "diff") == 0) {
            int unified = 0, side_by_side = 0, ignore_case = 0, context = -1;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--unified") == 0) {
                    unified = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--side-by-side") == 0) {
                    side_by_side = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--ignore-case") == 0) {
                    ignore_case = 1;
                } else if (strncmp(argv[i], "--context=", 10) == 0) {
                    context = atoi(argv[i] + 10);
                }
            }
            shark_diff(unified, side_by_side, ignore_case, context);
        } else if (fossil_io_cstring_compare(argv[i], "archive") == 0) {
            int create = 0, extract = 0;
            const char *format = cnullptr, *output = cnullptr;
            for (++i; i < argc && argv[i] != cnullptr; ++i) {
                if (fossil_io_cstring_compare(argv[i], "--create") == 0) {
                    create = 1;
                } else if (fossil_io_cstring_compare(argv[i], "--extract") == 0) {
                    extract = 1;
                } else if (strncmp(argv[i], "--format=", 9) == 0) {
                    format = argv[i] + 9;
                } else if (strncmp(argv[i], "--output=", 9) == 0) {
                    output = argv[i] + 9;
                }
            }
            shark_archive(create, extract, format, output);
        } else {
            fossil_io_printf("{red}Unknown command: %s{reset}\n", argv[i]);
            return false;
        }
    }
    return 0;
}
