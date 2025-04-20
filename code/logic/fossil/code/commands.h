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
 * Prints the application version.
 */
void handle_version(void);

/**
 * Prints the application name.
 */
void handle_name(void);

/**
 * Executes the 'move' command to move a file or directory from source to destination.
 *
 * @param source The source path.
 * @param destination The destination path.
 */
void handle_move(const char *source, const char *destination);

/**
 * Executes the 'copy' command to copy a file or directory from source to destination.
 *
 * @param source The source path.
 * @param destination The destination path.
 */
void handle_copy(const char *source, const char *destination);

/**
 * Executes the 'delete' command to delete a file or directory.
 *
 * @param target The target path to delete.
 */
void handle_delete(const char *target);

/**
 * Executes the 'list' command to list contents of a directory.
 *
 * @param directory The directory path to list.
 */
void handle_list(const char *directory);

/**
 * Executes the 'show' command to display the contents of a file.
 *
 * @param file The file path to display.
 */
void handle_show(const char *file);

/**
 * Executes the 'meta' command to display metadata of a file or directory.
 *
 * @param target The target path to retrieve metadata for.
 */
void handle_meta(const char *target);

/**
 * Executes the 'compress' command to compress a file or directory into an archive.
 *
 * @param source The source path to compress.
 * @param archive The archive file path to create.
 */
void handle_compress(const char *source, const char *archive);

/**
 * Executes the 'extract' command to extract an archive into a destination.
 *
 * @param archive The archive file path to extract.
 * @param destination The destination path to extract to.
 */
void handle_extract(const char *archive, const char *destination);

/**
 * Executes the 'chmod' command to change permissions of a file or directory.
 *
 * @param target The target path to change permissions for.
 * @param permissions The permissions to set.
 */
void handle_chmod(const char *target, const char *permissions);

/**
 * Executes the 'ownership' command to change the owner of a file or directory.
 *
 * @param target The target path to change ownership for.
 * @param owner The new owner to set.
 */
void handle_ownership(const char *target, const char *owner);

/**
 * Executes the 'find' command to search for files or directories matching a pattern.
 *
 * @param directory The directory path to search in.
 * @param pattern The pattern to search for.
 */
void handle_find(const char *directory, const char *pattern);

/**
 * Executes the 'search' command to search for a pattern within a file.
 *
 * @param file The file path to search in.
 * @param pattern The pattern to search for.
 */
void handle_search(const char *file, const char *pattern);

/**
 * Executes the 'size' command to display the size of a file or directory.
 *
 * @param target The target path to retrieve size for.
 */
void handle_size(const char *target);

/**
 * Executes the 'disk' command to display disk usage information for a path.
 *
 * @param path The path to retrieve disk usage information for.
 */
void handle_disk(const char *path);

/**
 * Executes the 'tree' command to display the directory structure as a tree.
 *
 * @param directory The directory path to display as a tree.
 */
void handle_tree(const char *directory);

/**
 * Executes the 'compare' command to compare two files or directories.
 *
 * @param path1 The first path to compare.
 * @param path2 The second path to compare.
 */
void handle_compare(const char *path1, const char *path2);

/**
 * Executes the 'sync' command to synchronize files or directories between source and destination.
 *
 * @param source The source path to synchronize from.
 * @param destination The destination path to synchronize to.
 */
void handle_sync(const char *source, const char *destination);

/**
 * Executes the 'update' command to update a file or directory.
 *
 * @param target The target path to update.
 */
void handle_update(const char *target);

/**
 * Executes the 'open' command to open a file.
 *
 * @param file The file path to open.
 */
void handle_open(const char *file);

/**
 * Executes the 'edit' command to edit a file.
 *
 * @param file The file path to edit.
 */
void handle_edit(const char *file);

/**
 * Executes the 'push' command to push changes from a source to a remote location.
 *
 * @param source The source path to push from.
 * @param remote The remote location to push to.
 */
void handle_push(const char *source, const char *remote);

/**
 * Executes the 'pull' command to pull changes from a remote location to a destination.
 *
 * @param remote The remote location to pull from.
 * @param destination The destination path to pull to.
 */
void handle_pull(const char *remote, const char *destination);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_LIFECYCLE_H */
