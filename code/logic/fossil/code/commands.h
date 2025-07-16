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
 * Create new files or directories.
 * Options:
 *   --type, --permissions
 */
void shark_create(const char *type, const char *permissions);

/**
 * Delete files or directories.
 * Options:
 *   --recursive, --force
 */
void shark_delete(bool recursive, bool force);

/**
 * Move or rename files and directories.
 * Options:
 *   --force, --backup
 */
void shark_move(bool force, bool backup);

/**
 * Rename files or directories.
 * Options:
 *   --force, --backup
 */
void shark_rename(bool force, bool backup);

/**
 * Copy files or directories.
 * Options:
 *   --recursive, --preserve
 */
void shark_copy(bool recursive, bool preserve);

/**
 * List files and directories.
 * Customize output using sort and format.
 * Options:
 *   --sort, --format
 */
void shark_list(const char *sort, const char *format);

/**
 * Display contents of a file, with options for viewing parts of it.
 * Options:
 *   --lines, --offset
 */
void shark_show(int lines, int offset);

/**
 * Find files or directories matching specific criteria (name, size, type, etc.).
 * Options:
 *   --name, --size, --type
 */
void shark_find(const char *name, const char *size, const char *type);

/**
 * Locate files or directories by name or path.
 * Options:
 *   --name, --path
 */
void shark_where(const char *name, const char *path);

/**
 * Search file contents for patterns.
 * Options:
 *   --pattern, --ignore-case
 */
void shark_search(const char *pattern, bool ignore_case);

/**
 * Create backups of files or directories with optional compression.
 * Options:
 *   --destination, --compress
 */
void shark_backup(const char *destination, bool compress);

/**
 * Display size of files or directories.
 * Options:
 *   --human-readable, --total
 */
void shark_size(bool human_readable, bool total);

/**
 * Display disk usage and free space.
 * Options:
 *   --all, --free
 */
void shark_disk(bool all, bool free);

/**
 * Display directory structure as a tree.
 * Options:
 *   --depth, --all
 */
void shark_tree(int depth, bool all);

/**
 * Compare two files or directories.
 * Options:
 *   --ignore-case, --binary
 */
void shark_compare(bool ignore_case, bool binary);

/**
 * Set color output mode.
 * Options:
 *   enable, disable, auto
 */
void shark_color(const char *mode);

/**
 * Get detailed information about a file or directory.
 * Options:
 *   --details, --type
 */
void shark_info(const char *details, const char *type);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_CLI_CODE_H */
