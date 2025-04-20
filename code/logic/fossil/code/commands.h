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
#ifndef FOSSIL_APP_COMMANDS_H
#define FOSSIL_APP_COMMANDS_H

#include <fossil/sys/framework.h>
#include <fossil/io/framework.h>

#define FOSSIL_APP_NAME "Shark Tool"
#define FOSSIL_APP_VERSION "0.1.0"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Displays the general help message for available commands.
 */
void handle_help(void);

/**
 * Prints the application version.
 */
void handle_version(void);

/**
 * Prints the application name.
 */
void handle_name(void);

/**
 * Moves a file or directory from source to destination.
 *
 * @param source The source path.
 * @param destination The destination path.
 */
void handle_move(const char *source, const char *destination);

/**
 * Copies a file or directory from source to destination.
 *
 * @param source The source path.
 * @param destination The destination path.
 */
void handle_copy(const char *source, const char *destination);

/**
 * Deletes a file or directory.
 *
 * @param target The target path to delete.
 */
void handle_delete(const char *target);

/**
 * Lists the contents of a directory.
 *
 * @param directory The directory path to list.
 */
void handle_list(const char *directory);

/**
 * Displays the contents of a file.
 *
 * @param file The file path to display.
 */
void handle_show(const char *file);

/**
 * Displays metadata for a file or directory.
 *
 * @param target The target path to retrieve metadata for.
 */
void handle_meta(const char *target);

/**
 * Compresses a file or directory into an archive.
 *
 * @param source The source path to compress.
 * @param archive The archive file path to create.
 */
void handle_compress(const char *source, const char *archive);

/**
 * Extracts an archive to a specified destination.
 *
 * @param archive The archive file to extract.
 * @param destination The destination path to extract to.
 */
void handle_extract(const char *archive, const char *destination);

/**
 * Changes permissions of a file or directory.
 *
 * @param target The target path.
 * @param permissions The permissions string (e.g., "755").
 */
void handle_chmod(const char *target, const char *permissions);

/**
 * Changes the owner of a file or directory.
 *
 * @param target The target path.
 * @param owner The new owner to assign.
 */
void handle_ownership(const char *target, const char *owner);

/**
 * Searches a directory for entries matching a pattern.
 *
 * @param directory The directory to search.
 * @param pattern The pattern to match.
 */
void handle_find(const char *directory, const char *pattern);

/**
 * Searches a file for lines matching a pattern.
 *
 * @param file The file to search.
 * @param pattern The pattern to search for.
 */
void handle_search(const char *file, const char *pattern);

/**
 * Displays the size of a file or directory.
 *
 * @param target The target path.
 */
void handle_size(const char *target);

/**
 * Displays disk usage statistics for a given path.
 *
 * @param path The path to analyze.
 */
void handle_disk(const char *path);

/**
 * Displays the directory tree structure of a given directory.
 *
 * @param directory The directory to walk recursively.
 */
void handle_tree(const char *directory);

/**
 * Compares two files or directories.
 *
 * @param path1 The first path.
 * @param path2 The second path.
 */
void handle_compare(const char *path1, const char *path2);

/**
 * Synchronizes contents from source to destination.
 *
 * @param source The source path.
 * @param destination The destination path.
 */
void handle_sync(const char *source, const char *destination);

/**
 * Updates a file or directory with the latest version.
 *
 * @param target The target path.
 */
void handle_update(const char *target);

/**
 * Opens a file using the system default application.
 *
 * @param file The file path to open.
 */
void handle_open(const char *file);

/**
 * Opens a file in a text editor.
 *
 * @param file The file path to edit.
 */
void handle_edit(const char *file);

/**
 * Pushes changes from a source path to a remote location.
 *
 * @param source The source path.
 * @param remote The remote destination.
 */
void handle_push(const char *source, const char *remote);

/**
 * Pulls content from a remote location into a destination.
 *
 * @param remote The remote source.
 * @param destination The local destination path.
 */
void handle_pull(const char *remote, const char *destination);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_LIFECYCLE_H */
