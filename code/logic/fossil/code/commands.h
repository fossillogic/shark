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
 * @brief Create new files or directories with specified type and name.
 * @param type The type of the entity to create (file/dir).
 * @param name The name of the file or directory to create.
 * @param file The path where the file or directory should be created.
 */
void shark_create(const char *file, const char *type, const char *name);

/**
 * @brief Remove files or directories. Optionally move them to trash instead of permanent deletion.
 * @param force If true, force deletion without confirmation.
 * @param file The path of the file or directory to delete.
 */
void shark_delete(const char *file, bool force);

/**
 * @brief Move or rename files and directories with options to overwrite or create backups.
 * @param source The source path of the file or directory to move.
 * @param destination The destination path where the file or directory should be moved.
 * @param force If true, overwrite existing files.
 * @param backup If true, create backups of overwritten files.
 */
void shark_move(const char *source, const char *destination, bool force, bool backup);

/**
 * @brief Rename files or directories with conflict handling options.
 * @param old_name The current name of the file or directory.
 * @param new_name The new name for the file or directory.
 * @param force If true, overwrite existing files.
 * @param backup If true, create backups of overwritten files.
 */
void shark_rename(const char *old_name, const char *new_name, bool force, bool backup);

/**
 * @brief Duplicate files or directories while preserving attributes or creating links.
 * @param source The source path of the file or directory to copy.
 * @param destination The destination path where the copy should be created.
 * @param preserve If true, preserve file attributes.
 * @param file_link The type of link to create (hard/sym).
 */
void shark_copy(const char *source, const char *destination, bool preserve, const char *file_link);

/**
 * @brief Display files or directories in a list or tree format with sorting options.
 * @param path The path of the directory to display.
 * @param as The format to display (list/tree).
 * @param sort The sorting order (disc/asc).
 */
void shark_list(const char *path, const char *as, const char *sort);

/**
 * @brief View file contents with support for pagination and line offsets.
 * @param file The path of the file to view.
 * @param lines The number of lines to display.
 * @param offset The line offset to start displaying from.
 */
void shark_show(const char *file, int lines, int offset);

/**
 * @brief Search for files or directories by name, type, or other criteria.
 * @param path The path to search within.
 * @param name The name pattern to search for.
 * @param type The type of entity to search for (file/dir).
 */
void shark_find(const char *path, const char *name, const char *type);

/**
 * @brief Show the size of files or directories with optional human-readable formatting.
 * @param path The path of the file or directory to check.
 * @param human_readable If true, format sizes in a human-readable way.
 */
void shark_size(const char *path, bool human_readable);

/**
 * @brief Compare files or directories using binary, diff, or hash methods.
 * @param path1 The path of the first file or directory to compare.
 * @param path2 The path of the second file or directory to compare.
 * @param binary If true, perform a binary comparison.
 * @param diff If true, perform a diff comparison.
 * @param hash If true, perform a hash comparison.
 */
void shark_compare(const char *path1, const char *path2, bool binary, bool diff, bool hash);

/**
 * @brief Verify the existence or type of files or directories.
 * @param exists Path to check for existence.
 * @param not_exist Path to check for non-existence.
 * @param type The type of entity to verify (file/dir).
 */
void shark_ask(const char *exists, const char *not_exist, const char *type);

/**
 * @brief Update file properties such as permissions, ownership, or mode.
 * @param target The path of the file or directory to modify.
 * @param value The new value to set.
 * @param owner If true, update ownership.
 * @param mode If true, update permissions/mode.
 */
void shark_change(const char *target, const char *value, bool owner, bool mode);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_CLI_CODE_H */
