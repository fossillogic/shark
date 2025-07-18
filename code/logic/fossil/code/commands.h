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
#ifndef FOSSIL_CLI_CODE_H
#define FOSSIL_CLI_CODE_H

#include <fossil/sys/framework.h>
#include <fossil/io/framework.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create new files or directories with specific type and permissions.
 * Flags: --type=<file/dir>, --permissions=<rwx>
 */
void shark_create(const char *path, const char *type, const char *permissions);

/**
 * Delete files or directories. Optionally move to trash instead of hard delete.
 * Flags: --recursive, --force, --trash
 */
void shark_delete(const char *path, bool recursive, bool force, bool trash);

/**
 * Move or rename files and directories with overwrite and backup options.
 * Flags: --force, --backup, --overwrite
 */
void shark_move(const char *source, const char *destination, bool force, bool backup, bool overwrite);

/**
 * Rename files or directories with conflict resolution.
 * Flags: --force, --backup
 */
void shark_rename(const char *old_name, const char *new_name, bool force, bool backup);

/**
 * Copy files or directories with control over attributes and link types.
 * Flags: --recursive, --preserve, --symlinks, --hard
 */
void shark_copy(const char *source, const char *destination, bool recursive, bool preserve, bool symlinks, bool hard);

/**
 * List system information. Defaults to file listing. Supports devices, kernel modules, and hardware info.
 * Flags: --what=<files/devices/modules/hardware>, --sort, --format
 */
void shark_list(const char *path, const char *what, bool sort, const char *format);

/**
 * Show file contents with pagination and range options.
 * Flags: --lines=<n>, --offset=<n>, --tail, --head
 */
void shark_show(const char *file, int lines, int offset, bool tail, bool head);

/**
 * Find items by name, size, or type.
 * Flags: --name=<pattern>, --size=<gt/lt>:<value>, --type
 */
void shark_find(const char *path, const char *name, const char *size, const char *type);

/**
 * Locate files or directories by name or full path pattern.
 * Flags: --name=<pattern>, --path=<regex>
 */
void shark_where(const char *path, const char *name, const char *regex_path);

/**
 * Search inside files with advanced pattern matching.
 * Flags: --pattern=<regex>, --ignore-case, --whole-word
 */
void shark_search(const char *file, const char *pattern, bool ignore_case, bool whole_word);

/**
 * Backup files or directories with optional compression and encryption.
 * Flags: --destination=<path>, --compress, --encrypt
 */
void shark_backup(const char *source, const char *destination, bool compress, bool encrypt);

/**
 * Display size of items with formatting and summary options.
 * Flags: --human-readable, --total, --summarize
 */
void shark_size(const char *path, bool human_readable, bool total, bool summarize);

/**
 * Show disk statistics including inode usage.
 * Flags: --all, --free, --used, --inodes
 */
void shark_disk(const char *path, bool all, bool free, bool used, bool inodes);

/**
 * Print directory tree with control over depth and content.
 * Flags: --depth=<n>, --all, --dirs-only, --files-only
 */
void shark_tree(const char *path, int depth, bool all, bool dirs_only, bool files_only);

/**
 * Compare files or directories using various strategies.
 * Flags: --ignore-case, --binary, --diff, --hash
 */
void shark_compare(const char *source, const char *target, bool ignore_case, bool binary, bool diff, bool hash);

/**
 * Display metadata and statistics about a file or directory.
 * Flags: --details, --type, --stat, --checksum
 */
void shark_info(const char *path, bool details, const char *type, bool stat, bool checksum);

/**
 * Clean up generated or temporary files, with optional preview.
 * Flags: --temp, --cache, --logs, --dry-run
 */
void shark_clean(const char *path, bool temp, bool cache, bool logs, bool dry_run);

/**
 * Perform basic file operations. Includes splitting and joining files.
 * Flags: --create, --modify, --delete, --split=<lines/bytes>, --join=<file1,file2,...>, --output=<file>
 */
void shark_file(const char *path, bool create, bool modify, bool delete_op, const char *split, const char *join, const char *output);

/**
 * Check for file/directory existence or type.
 * Flags: --exists=<path>, --not-exist, --type=<file/dir>
 */
void shark_ask(const char *path, const char *exists, bool not_exist, const char *type);

/**
 * Modify file properties like permissions or ownership.
 * Flags: --target=<path>, --value=<new>, --owner, --mode
 */
void shark_change(const char *target, const char *value, bool owner, bool mode);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_CLI_CODE_H */
