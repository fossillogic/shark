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
 * Options:
 *   --type=<file/dir>
 *   --permissions=<rwx>
 */
void shark_create(const char *type, const char *permissions);

/**
 * Delete files or directories. Optionally move to trash instead of hard delete.
 * Options:
 *   --recursive
 *   --force
 *   --trash
 */
void shark_delete(int recursive, int force, int trash);

/**
 * Move or rename files and directories with overwrite and backup options.
 * Options:
 *   --force
 *   --backup
 *   --overwrite
 */
void shark_move(const char *source, const char *destination, int force, int backup, int overwrite);

/**
 * Rename files or directories with conflict resolution.
 * Options:
 *   --force
 *   --backup
 */
void shark_rename(const char *old_name, const char *new_name, int force, int backup);

/**
 * Copy files or directories with control over attributes and link types.
 * Options:
 *   --recursive
 *   --preserve
 *   --symlinks
 *   --hard
 */
void shark_copy(const char *source, const char *destination, int recursive, int preserve, int symlinks, int hard);

/**
 * List system information. Defaults to file listing. Supports devices, kernel modules, and hardware info.
 * Options:
 *   --what=<files/devices/modules/hardware>
 *   --sort
 *   --format
 */
void shark_list(const char *what, int sort, const char *format);

/**
 * Show file contents with pagination and range options.
 * Options:
 *   --lines=<n>
 *   --offset=<n>
 *   --tail
 *   --head
 */
void shark_show(const char *file, int lines, int offset, int tail, int head);

/**
 * Find items by name, size, or type.
 * Options:
 *   --name=<pattern>
 *   --size=<gt/lt>:<value>
 *   --type
 */
void shark_find(const char *directory, const char *name, const char *size, const char *type);

/**
 * Locate files or directories by name or full path pattern.
 * Options:
 *   --name=<pattern>
 *   --path=<regex>
 */
void shark_where(const char *directory, const char *name, const char *path);

/**
 * Search inside files with advanced pattern matching.
 * Options:
 *   --pattern=<regex>
 *   --ignore-case
 *   --whole-word
 */
void shark_search(const char *file, const char *pattern, int ignore_case, int whole_word);

/**
 * Backup files or directories with optional compression and encryption.
 * Options:
 *   --destination=<path>
 *   --compress
 *   --encrypt
 */
void shark_backup(const char *source, const char *destination, int compress, int encrypt);

/**
 * Display size of items with formatting and summary options.
 * Options:
 *   --human-readable
 *   --total
 *   --summarize
 */
void shark_size(const char *path, int human_readable, int total, int summarize);

/**
 * Show disk statistics including inode usage.
 * Options:
 *   --all
 *   --free
 *   --used
 *   --inodes
 */
void shark_disk(const char *path, int all, int free, int used, int inodes);

/**
 * Print directory tree with control over depth and content.
 * Options:
 *   --depth=<n>
 *   --all
 *   --dirs-only
 *   --files-only
 */
void shark_tree(const char *directory, int depth, int all, int dirs_only, int files_only);

/**
 * Compare files or directories using various strategies.
 * Options:
 *   --ignore-case
 *   --binary
 *   --diff
 *   --hash
 */
void shark_compare(const char *path1, const char *path2, int ignore_case, int binary, int diff, int hash);

/**
 * Display metadata and statistics about a file or directory.
 * Options:
 *   --details
 *   --type
 *   --stat
 *   --checksum
 */
void shark_info(const char *path, int details, const char *type, int stat, int checksum);

/**
 * Clean up generated or temporary files, with optional preview.
 * Options:
 *   --temp
 *   --cache
 *   --logs
 *   --dry-run
 */
void shark_clean(const char *directory, int temp, int cache, int logs, int dry_run);

/**
 * Perform basic file operations. Includes splitting and joining files.
 * Options:
 *   --create
 *   --modify
 *   --delete
 *   --split=<lines/bytes>
 *   --join=<file1,file2,...>
 *   --output=<file>
 */
void shark_file(const char *path, int create, int modify, int delete, const char *split, const char *join, const char *output);

/**
 * Check for file/directory existence or type.
 * Options:
 *   --exists=<path>
 *   --not-exist
 *   --type=<file/dir>
 */
void shark_ask(const char *exists, int not_exist, const char *type);

/**
 * Modify file properties like permissions or ownership.
 * Options:
 *   --target=<path>
 *   --value=<new>
 *   --owner
 *   --mode
 */
void shark_change(const char *target, const char *value, int owner, int mode);

/**
 * Show textual differences between files. Supports unified and side-by-side views.
 * Options:
 *   --unified
 *   --side-by-side
 *   --ignore-case
 *   --context=<n>
 */
void shark_diff(const char *file1, const char *file2, int unified, int side_by_side, int ignore_case, int context);

/**
 * Create or extract archive files in supported formats.
 * Options:
 *   --create
 *   --extract
 *   --format=zip/tar/gz
 *   --output=<file>
 */
void shark_archive(const char *source, const char *destination, int create, int extract, const char *format, const char *output);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_CLI_CODE_H */
